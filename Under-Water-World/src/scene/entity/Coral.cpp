#include <GL/glew.h>
#include "scene/entity/Coral.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Coral::Coral()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = 1.0f;
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    shaderProgram = 0;
}

bool Coral::init(
    const char* modelPath,
    const char* vertexShaderPath,
    const char* fragmentShaderPath
)
{
    shaderProgram = shaderLoader.CreateProgram(
        vertexShaderPath,
        fragmentShaderPath
    );

    if (shaderProgram == 0)
    {
        std::cout << "Failed to create Coral shader program" << std::endl;
        return false;
    }

    if (!model.loadFromFile(modelPath))
    {
        return false;
    }

    return true;
}

void Coral::update(float deltaTime, const Input& input)
{
}

void Coral::render(
    const glm::mat4& view,
    const glm::mat4& projection,
    float time,
    glm::vec3 cameraPos,
    glm::vec3 fogColor,
    float fogDensity,
    glm::vec3 lightPos
)
{
    glUseProgram(shaderProgram);

    glm::mat4 modelMatrix = getModelMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), time);
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));

    model.draw();
}

void Coral::renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix)
{
    glUseProgram(depthShader);

    glm::mat4 modelMatrix = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    model.drawDepth();
}

glm::mat4 Coral::getModelMatrix() const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::translate(modelMatrix, pivotOffset);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    return modelMatrix;
}

void Coral::shutdown()
{
    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Coral::getPosition() const
{
    return position;
}

void Coral::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

void Coral::setScale(float newScale)
{
    scale = newScale;
}

void Coral::setRotation(glm::vec3 newRotation)
{
    rotation = newRotation;
}

void Coral::setPivotOffset(glm::vec3 offset)
{
    pivotOffset = offset;
}