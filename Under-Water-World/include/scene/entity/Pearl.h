#pragma once

#include "graphics/Render_Utils.h"
#include "graphics/Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

class Pearl
{
public:
    Pearl();

    bool init(
        const char* vertexShaderPath,
        const char* fragmentShaderPath
    );

    void render(
        const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos,
        GLuint cubemapTextureId,
        float time,
        const glm::vec3& fogColor,
        float fogDensity,
        const glm::mat4& lightSpaceMatrix,
        GLuint shadowMap,
        bool useShadows
    );

    void renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix);

    void shutdown();

    void setPosition(const glm::vec3& newPosition);
    void setScale(float newScale);

    glm::vec3& getBaseColor();
    float& getF0();
    float& getFresnelPower();
    float& getRefractionRatio();
    float& getReflectionStrength();
    float& getRefractionStrength();
    void resetParameters();

private:
    void generateSphere();
    glm::mat4 getModelMatrix() const;

private:
    glm::vec3 position;
    float scale;

    glm::vec3 baseColor;
    float fresnelPower;
    float refractionRatio;
    float reflectionStrength;
    float refractionStrength;
    float f0;

    GLuint shaderProgram;
    Core::Shader_Loader shaderLoader;

    Core::RenderContext sphereContext;
};
