#pragma once
#include "graphics/Render_Utils.h"
#include "graphics/Texture.h"
#include <vector>

struct MeshData {
    Core::RenderContext context;
    Texture texture;
};

class Model
{
public:
    Model();
    bool loadFromFile(const char* path);
    void draw();
    void drawDepth();

private:
    std::vector<MeshData> meshes;
};
