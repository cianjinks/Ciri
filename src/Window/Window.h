#ifndef CIRI_WINDOW_H
#define CIRI_WINDOW_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Event.h"

namespace Ciri
{
    class Window
    {
        using CallbackFunc = std::function<void(Event&)>;
    public:
        std::string Name;
        int32_t Width;
        int32_t Height;

    private:
        GLFWwindow *m_Window;
        CallbackFunc m_EventCallback;

        bool m_CursorCaptured = false;

        float m_LastFrameTime = 0.0f;
        float m_DeltaTime = 0.0f;

    public:
        Window(std::string name, int32_t width, int32_t height);
        ~Window();
        GLFWwindow* GetWindow() { return m_Window; }

        void OnUpdate();
        void OnEvent(Event& event);

        void SetEventCallback(CallbackFunc callback) { m_EventCallback = callback; }

        bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

        void CaptureCursor();
        void ReleaseCursor();
        bool IsCursorCaptured() { return m_CursorCaptured; }

        glm::ivec2 GetMainMonitorResolution();
        int GetMainMonitorWidth();
        int GetMainMonitorHeight();
        
        float GetTimeStep() { return m_DeltaTime; }

    private:
        static Window* s_Instance;
    public:
        static Window* Get() { return s_Instance; }
    };
}

#endif