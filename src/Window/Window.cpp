#include "Window.h"

namespace Ciri
{
    Window* Window::s_Instance = nullptr;

    Window::Window(std::string name, int32_t width, int32_t height)
        : Name(name), Width(width), Height(height)
    {
        if(s_Instance)
        {
            CIRI_ERROR("Only one instance of a Window may exist!");
        }
        s_Instance = this;

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
        
        // Event Callbacks
        glfwSetWindowUserPointer(m_Window, &m_EventCallback);

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            CallbackFunc fn = *static_cast<CallbackFunc*>(glfwGetWindowUserPointer(window));
            if (!fn) { CIRI_ERROR("No Event Callback function is set"); }
            
            switch(action)
            {
                case GLFW_PRESS:
                {
                    KeyEvent event(EventType::KEY_PRESS, key);
                    fn(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyEvent event(EventType::KEY_RELEASE, key);
                    fn(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyEvent event(EventType::KEY_REPEAT, key);
                    fn(event);
                    break;
                }
            }
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
        {
            CallbackFunc fn = *static_cast<CallbackFunc*>(glfwGetWindowUserPointer(window));
            if (!fn) { CIRI_ERROR("No Event Callback function is set"); }

            MousePositionEvent event(xpos, ypos);
            fn(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            CallbackFunc fn = *static_cast<CallbackFunc*>(glfwGetWindowUserPointer(window));
            if (!fn) { CIRI_ERROR("No Event Callback function is set"); }

            switch(action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonEvent event(EventType::MOUSE_PRESS, button);
                    fn(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonEvent event(EventType::MOUSE_RELEASE, button);
                    fn(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            CallbackFunc fn = *static_cast<CallbackFunc*>(glfwGetWindowUserPointer(window));
            if (!fn) { CIRI_ERROR("No Event Callback function is set"); }

            MouseScrollEvent event(xOffset, yOffset);
            fn(event);
        });

        CIRI_LOG("Window & OpenGL Context Initialised");
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::OnUpdate()
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    void Window::OnEvent(Event& event)
    {
        EventType type = event.GetEventType();
        switch (type)
        {
            case EventType::MOUSE_PRESS:
                MouseButtonEvent& mouse_event = static_cast<MouseButtonEvent&>(event);
                int button = mouse_event.GetButton();
                if (button == GLFW_MOUSE_BUTTON_LEFT)
                {
                    CaptureCursor();
                }
                if (button == GLFW_MOUSE_BUTTON_RIGHT)
                {
                    ReleaseCursor();
                }
                break;
        }
    }

    void Window::CaptureCursor()
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_CursorCaptured = true;
    }

    void Window::ReleaseCursor()
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_CursorCaptured = false;
    }

    float Window::CalcDeltaTime()
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - m_LastFrameTime;
        m_LastFrameTime = currentFrame;
        return deltaTime;
    }

    glm::ivec2 Window::GetMainMonitorResolution()
    {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        return {mode->width, mode->height};
    }

    int Window::GetMainMonitorWidth()
    {
        return GetMainMonitorResolution().x;
    }

    int Window::GetMainMonitorHeight()
    {
        return GetMainMonitorResolution().y;
    }
}