#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window/Input.h"

namespace Ciri
{
    Camera::Camera(glm::vec3 pos, glm::vec3 direction, float yaw, float pitch, float width, float height)
        : Position(pos), Front(direction), Yaw(yaw), Pitch(pitch), m_Width(width), m_Height(height)
    {
    }

    void Camera::OnUpdate(float dt)
    {
        if (!Window::Get()->IsCursorCaptured())
        {
            return;
        }

        if (Input::IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        {
            Speed = SpeedHigh * dt;
        }
        else
        {
            Speed = SpeedLow * dt;
        }

        if (Input::IsKeyPressed(GLFW_KEY_W))
        {
            Position += Speed * Front;
        }
        if (Input::IsKeyPressed(GLFW_KEY_S))
        {
            Position -= Speed * Front;
        }
        if (Input::IsKeyPressed(GLFW_KEY_A))
        {
            Position -= glm::normalize(glm::cross(Front, Up)) * Speed;
        }
        if (Input::IsKeyPressed(GLFW_KEY_D))
        {
            Position += glm::normalize(glm::cross(Front, Up)) * Speed;
        }
    }

    void Camera::OnEvent(Event& event)
    {
        if(!Window::Get()->IsCursorCaptured())
        {
            return;
        }

        EventType type = event.GetEventType();
        switch (type)
        {
            case EventType::WINDOW_RESIZE:
            {
                WindowResizeEvent& resize_event = static_cast<WindowResizeEvent&>(event);
                Resize(resize_event.GetWidth(), resize_event.GetHeight());
                break;
            }
            case EventType::MOUSE_POSITION:
                MousePositionEvent& mouse_pos = static_cast<MousePositionEvent&>(event);
                double mouseX = mouse_pos.GetXPos();
                double mouseY = mouse_pos.GetYPos();

                if (FirstMouse)
                {
                    LastX = (float)mouseX;
                    LastY = (float)mouseY;
                    FirstMouse = false;
                }

                float xoffset = (float)mouseX - LastX;
                float yoffset = LastY - (float)mouseY;
                LastX = (float)mouseX;
                LastY = (float)mouseY;

                float sensitivity = 0.05f;
                xoffset *= sensitivity;
                yoffset *= sensitivity;

                Yaw += xoffset;
                Pitch += yoffset;

                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;

                RecalcDirection();
        }
    }

    void Camera::Resize(int32_t width, int32_t height)
    {
        m_Width = (float)width;
        m_Height = (float)height;
    }

    void Camera::RecalcDirection()
    {
        Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front.y = sin(glm::radians(Pitch));
        Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(Front);
    }

    void Camera::RecalcVP()
    {
        m_ProjectionMat = glm::perspective(glm::radians(FOV), m_Width / m_Height, Near, Far);
        m_ViewMat = glm::lookAt(Position, Position + Front, Up);
        m_InvProjMat = glm::inverse(m_ProjectionMat);
        m_InvViewMat = glm::inverse(m_ViewMat);
    }
}