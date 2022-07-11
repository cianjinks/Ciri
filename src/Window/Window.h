#ifndef CIRI_WINDOW_H
#define CIRI_WINDOW_H

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Ciri
{
    class Window
    {
    public:
        std::string Name;
        int32_t Width;
        int32_t Height;

    private:
        GLFWwindow *m_Window;

    public:
        Window(std::string name, int32_t width, int32_t height);
        ~Window();

        void Update();

        bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
    };
}

#endif