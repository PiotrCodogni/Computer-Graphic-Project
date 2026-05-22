#pragma once

#include "scene/Scene.h"

class Renderer
{
public:
    Renderer();

    bool init();
    void beginFrame();
    void render(Scene& scene);
    void endFrame();
    void shutdown();
};