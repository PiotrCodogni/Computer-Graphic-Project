#include "graphics/Renderer.h"

#include <GL/glew.h>

Renderer::Renderer()
{
}

bool Renderer::init()
{
    glEnable(GL_DEPTH_TEST);

    return true;
}

void Renderer::beginFrame()
{
   
    glClearColor(0.0f, 0.25f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render(Scene& scene)
{
    float aspectRatio = 1.0f;

    glm::mat4 view = scene.getCamera().getViewMatrix();
    glm::mat4 projection = scene.getCamera().getProjectionMatrix(aspectRatio);

    scene.getFish().render(view, projection);
}

void Renderer::endFrame()
{
}

void Renderer::shutdown()
{
}