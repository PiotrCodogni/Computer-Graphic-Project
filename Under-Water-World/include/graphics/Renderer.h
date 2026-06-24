#pragma once

#include "scene/Scene.h"
#include "graphics/Shader.h"

#include <GL/glew.h>

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
    static constexpr int SHADOW_WIDTH = 2048;
    static constexpr int SHADOW_HEIGHT = 2048;

    int viewportWidth;
    int viewportHeight;

    GLuint depthMapFBO;
    GLuint depthMap;
    GLuint shadowDepthShader;
    GLuint shadowDepthFishShader;
    GLuint shadowDepthAlgaeShader;

    Core::Shader_Loader shaderLoader;
};
