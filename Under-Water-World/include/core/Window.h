#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window
{
public:
    Window();

    bool init(int width, int height, const char* title);
    void shutdown();

    bool shouldClose() const;
    void close();

    void swapBuffers();
    void pollEvents();

    GLFWwindow* getWindow() const;

    int getWidth() const;
    int getHeight() const;

private:
    GLFWwindow* window;

    int width;
    int height;
};