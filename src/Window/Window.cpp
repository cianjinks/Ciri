#include "Window.h"

namespace Ciri
{
    Window::Window(std::string name, int32_t width, int32_t height)
        : Name(name), Width(width), Height(height)
    {
        if (!glfwInit())
        {
            CIRI_ERROR("GLFW Initialization Failed");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

        m_Window = glfwCreateWindow(Width, Height, Name.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            glfwTerminate();
            CIRI_ERROR("GLFW Window Creation Failed");
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(0);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            CIRI_ERROR("GLAD Initialization Failed");
        }

        // TODO: Debug callback for OpenGL context
        CIRI_LOG("Window & OpenGL Context Initialised");
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::Update()
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}