#include "core/Input.h"
#include "core/Window.h"


Input::Input()
{
    window = nullptr;
}

void Input::update(Window& appWindow)
{
    window = appWindow.getWindow();
}

bool Input::isKeyPressed(int key) const
{
    if (window == nullptr)
    {
        return false;
    }

    return glfwGetKey(window, key) == GLFW_PRESS;
}