#include "Input.h"

#include <GLFW/glfw3.h>

namespace Ciri
{
    U<Input> Input::s_InputInstance = CreateU<Input>();

    void Input::Init(S<Window> window)
    {
        s_InputInstance->WindowRef = window;
    }

    bool Input::IsKeyPressed(int key)
    {
        GLFWwindow* window = s_InputInstance->WindowRef->GetWindow();
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

	bool Input::IsMouseButtonPressed(int button)
    {
        GLFWwindow* window = s_InputInstance->WindowRef->GetWindow();
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

	double Input::GetMouseX()
    {
        GLFWwindow* window = s_InputInstance->WindowRef->GetWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return xpos;
    }

	double Input::GetMouseY()
    {
        GLFWwindow* window = s_InputInstance->WindowRef->GetWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return ypos;
    }
}