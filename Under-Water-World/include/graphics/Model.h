#pragma once

#include "graphics/Render_Utils.h"

class Model
{
public:
    Model();

    bool loadFromFile(const char* path);
    void draw();

private:
    Core::RenderContext renderContext;
};