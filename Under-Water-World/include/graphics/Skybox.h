#pragma once

#include "graphics/Cubemap.h"
#include "graphics/Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

class Skybox
{
public:
    Skybox();

    bool init(const std::vector<std::string>& faces);
    void render(const glm::mat4& view, const glm::mat4& projection);
    void shutdown();

    GLuint getCubemapId() const;

private:
    GLuint vao;
    GLuint vbo;
    GLuint shaderProgram;

    Cubemap cubemap;
    Core::Shader_Loader shaderLoader;
};
