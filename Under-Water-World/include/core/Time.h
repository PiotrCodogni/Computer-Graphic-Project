#pragma once

class Time
{
public:
    Time();

    void update();

    float getDeltaTime() const;
    float getCurrentTime() const;

private:
    float lastFrameTime;
    float currentFrameTime;
    float deltaTime;
};