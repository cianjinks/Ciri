#ifndef CIRI_CAMERA_H
#define CIRI_CAMERA_H

#include <glm/glm.hpp>

namespace Ciri
{
    class Camera
    {
    public:
        glm::vec3 Position = glm::vec3(0.0f, 0.0f, 10.0f);
        glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

        float Yaw = 0.0f;
        float Pitch = 0.0f;

        float Speed = 15.0f;
        float SpeedHigh = 30.0f;
        float SpeedLow = 15.0f;
        float Far = 250.0f; // Render Distance
        float Near = 0.1f;
        float FOV = 45.0f;

        float LastX = 0.0f;
        float LastY = 0.0f;
        
    private:
        float m_Width, m_Height;

        glm::mat4 m_ProjectionMat;
        glm::mat4 m_ViewMat;
        glm::mat4 m_InvProjMat;
        glm::mat4 m_InvViewMat;

    public:
        Camera(glm::vec3 pos, glm::vec3 direction, float yaw, float pitch, float screenWidth, float screenHeight);
        ~Camera() = default;

        void OnUpdate(float dt);
        void OnEvent(Event& event);

        void Resize(int32_t width, int32_t height);
        
        void RecalcDirection();
        void RecalcVP();

        glm::mat4 GetProjectionMat() { return m_ProjectionMat; }
        glm::mat4 GetViewMat() { return m_ViewMat; }
        glm::mat4 GetInvProjMat() { return m_InvProjMat; }
        glm::mat4 GetInvViewMat() { return m_InvViewMat; }
    };
}

#endif