#pragma once

#include "scene/Scene.h"

class Renderer
{
public:
    Renderer();

    bool init(int width, int height);
    void shutdown();

    void beginFrame();
    void render(Scene& scene);
    void endFrame();

    void setViewportSize(int width, int height);


private:
    int viewportWidth;
    int viewportHeight;
};