#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ciri
{
    Camera::Camera(glm::vec3 pos, glm::vec3 direction, float yaw, float pitch, float width, float height)
        : Position(pos), Front(direction), Yaw(yaw), Pitch(pitch), m_Width(width), m_Height(height)
    {
    }

    void Camera::RecalcDirection()
    {
        Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front.y = sin(glm::radians(Pitch));
        Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(Front);
    }

    void Camera::RecalcMVP()
    {
        m_ProjectionMat = glm::perspective(glm::radians(FOV), m_Width / m_Height, 0.1f, RenderDist);
        m_ViewMat = glm::lookAt(Position, Position + Front, Up);
        m_ModelMat = glm::mat4(1.0f);
    }
}