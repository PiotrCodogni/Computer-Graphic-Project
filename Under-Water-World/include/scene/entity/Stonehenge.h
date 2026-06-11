#pragma once

#include "core/Input.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include <GL/glew.h>
#include <glm/glm.hpp>

class Stonehenge
{
public:
    Stonehenge();

    bool init(
        const char* modelPath,
        const char* vertexShaderPath,
        const char* fragmentShaderPath
    );

    void update(float deltaTime, const Input& input);

    void render(
        const glm::mat4& view,
        const glm::mat4& projection,
        float time,
        glm::vec3 cameraPos,
        glm::vec3 fogColor,
        float fogDensity,
        glm::vec3 lightPos
    );

    void shutdown();

    glm::vec3 getPosition() const;

    void setPosition(glm::vec3 newPosition);
    void setScale(float newScale);
    void setRotation(glm::vec3 newRotation);
    void setPivotOffset(glm::vec3 offset);

private:
    glm::vec3 position;
    float scale;
    glm::vec3 rotation;
    glm::vec3 pivotOffset;

    GLuint shaderProgram;
    Core::Shader_Loader shaderLoader;

    Model model;
};