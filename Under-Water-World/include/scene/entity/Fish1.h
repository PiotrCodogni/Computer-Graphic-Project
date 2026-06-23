#pragma once

#include "core/Input.h"
#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

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
    void update(float deltaTime, const Input& input, const glm::vec3& cameraForward);
    void render(const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        const glm::vec3& lightPos,
        const glm::vec3& fogColor,
        float fogDensity,
        const glm::mat4& lightSpaceMatrix,
        GLuint shadowMap,
        bool useShadows
    );

    void renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix);

    void shutdown();

    glm::vec3 getPosition() const;

private:
    glm::mat4 getModelMatrix() const;

    glm::vec3 position;

    glm::quat rotation;
    float rotationSpeed;

    float speed;
    float scale;
    float animationTime;

    GLuint shaderProgram;

    Core::Shader_Loader shaderLoader;

    Model model;
    Texture texture;
};
