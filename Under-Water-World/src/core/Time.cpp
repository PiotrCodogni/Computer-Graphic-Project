#include "core/Time.h"

#include <GLFW/glfw3.h>

Time::Time()
{
    lastFrameTime = 0.0f;
    currentFrameTime = 0.0f;
    deltaTime = 0.0f;
}

void Time::update()
{
    currentFrameTime = static_cast<float>(glfwGetTime());
    deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;
}

float Time::getDeltaTime() const
{
    return deltaTime;
}

float Time::getCurrentTime() const
{
    return currentFrameTime;
}