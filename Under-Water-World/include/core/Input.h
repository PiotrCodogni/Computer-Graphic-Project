#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window;

class Input
{
public:
    Input();

    void update(Window& window);

    bool isKeyPressed(int key) const;

private:
    GLFWwindow* window;
};