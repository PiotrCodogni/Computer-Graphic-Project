#include "scene/entity/Pearl.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <vector>
#include <cmath>

Pearl::Pearl()
{
    position = glm::vec3(2.0f, -2.0f, -4.0f);
    scale = 0.8f;

    resetParameters();

    shaderProgram = 0;
}

bool Pearl::init(
    const char* vertexShaderPath,
    const char* fragmentShaderPath
)
{
    shaderProgram = shaderLoader.CreateProgram(vertexShaderPath, fragmentShaderPath);

    if (shaderProgram == 0)
    {
        std::cout << "Failed to create pearl shader." << std::endl;
        return false;
    }

    generateSphere();

    return true;
}

void Pearl::generateSphere()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    const int rings = 64;
    const int sectors = 64;
    const float radius = 1.0f;

    for (int r = 0; r <= rings; ++r)
    {
        float phi = glm::pi<float>() * static_cast<float>(r) / static_cast<float>(rings);

        for (int s = 0; s <= sectors; ++s)
        {
            float theta = glm::two_pi<float>() * static_cast<float>(s) / static_cast<float>(sectors);

            float x = std::cos(theta) * std::sin(phi);
            float y = std::cos(phi);
            float z = std::sin(theta) * std::sin(phi);

            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

            positions.push_back(normal * radius);
            normals.push_back(normal);
            uvs.push_back(glm::vec2(
                static_cast<float>(s) / static_cast<float>(sectors),
                static_cast<float>(r) / static_cast<float>(rings)
            ));
        }
    }

    for (int r = 0; r < rings; ++r)
    {
        for (int s = 0; s < sectors; ++s)
        {
            int current = r * (sectors + 1) + s;
            int next = current + sectors + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    sphereContext.initFromVectors(positions, normals, uvs, indices);
}

void Pearl::render(
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
)
{
    glUseProgram(shaderProgram);

    glm::mat4 modelMatrix = getModelMatrix();

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(modelMatrix)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"),
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "cameraPos"),
        1,
        glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "baseColor"),
        1,
        glm::value_ptr(baseColor)
    );

    glUniform1f(glGetUniformLocation(shaderProgram, "f0"), f0);
    glUniform1f(glGetUniformLocation(shaderProgram, "fresnelPower"), fresnelPower);
    glUniform1f(glGetUniformLocation(shaderProgram, "refractionRatio"), refractionRatio);
    glUniform1f(glGetUniformLocation(shaderProgram, "reflectionStrength"), reflectionStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "refractionStrength"), refractionStrength);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform1i(glGetUniformLocation(shaderProgram, "useShadows"), useShadows ? 1 : 0);

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "fogColor"),
        1,
        glm::value_ptr(fogColor)
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "fogDensity"),
        fogDensity
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureId);
    glUniform1i(glGetUniformLocation(shaderProgram, "environmentMap"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 5);

    Core::DrawContext(sphereContext);
}

void Pearl::renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix)
{
    glUseProgram(depthShader);

    glm::mat4 modelMatrix = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    Core::DrawContext(sphereContext);
}

glm::mat4 Pearl::getModelMatrix() const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    return modelMatrix;
}

void Pearl::shutdown()
{
    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

void Pearl::setPosition(const glm::vec3& newPosition)
{
    position = newPosition;
}

void Pearl::setScale(float newScale)
{
    scale = newScale;
}

glm::vec3& Pearl::getBaseColor()
{
    return baseColor;
}

float& Pearl::getF0()
{
    return f0;
}

float& Pearl::getFresnelPower()
{
    return fresnelPower;
}

float& Pearl::getRefractionRatio()
{
    return refractionRatio;
}

float& Pearl::getReflectionStrength()
{
    return reflectionStrength;
}

float& Pearl::getRefractionStrength()
{
    return refractionStrength;
}

void Pearl::resetParameters()
{
    baseColor = glm::vec3(1.0f, 0.92f, 0.82f);
    fresnelPower = 5.0f;
    refractionRatio = 0.66f;
    reflectionStrength = 0.9f;
    refractionStrength = 0.18f;
    f0 = 0.07f;
}
