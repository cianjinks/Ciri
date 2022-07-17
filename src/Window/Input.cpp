#include "Input.h"

#include <GLFW/glfw3.h>

namespace Ciri
{
    bool Input::IsKeyPressed(int key)
    {
        GLFWwindow* window = Window::Get()->GetWindow();
        return glfwGetKey(window, key) == GLFW_PRESS;
    }

	bool Input::IsMouseButtonPressed(int button)
    {
        GLFWwindow* window = Window::Get()->GetWindow();
        return glfwGetMouseButton(window, button) == GLFW_PRESS;
    }

    glm::dvec2 Input::GetMouseCoord()
    {
        GLFWwindow* window = Window::Get()->GetWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return {xpos, ypos};
    }

	double Input::GetMouseX()
    {
        return GetMouseCoord().x;
    }

	double Input::GetMouseY()
    {
        return GetMouseCoord().y;
    }
}