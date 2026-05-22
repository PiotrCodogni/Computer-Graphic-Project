#pragma once

#include "core/Input.h"
#include "graphics/Render_Utils.h"
#include "graphics/Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

class Fish
{
public:
    Fish();

    bool init(
        const char* modelPath,
        const char* texturePath,
        const char* vertexShaderPath,
        const char* fragmentShaderPath
    );

    void update(float deltaTime, const Input& input);

    void render(
        const glm::mat4& view,
        const glm::mat4& projection
    );

    void shutdown();

    glm::vec3 getPosition() const;

private:
    bool loadTexture(const char* texturePath);

private:
    glm::vec3 position;
    float speed;
    float scale;

    GLuint shaderProgram;
    GLuint textureId;

    Core::Shader_Loader shaderLoader;
    Core::RenderContext renderContext;
};