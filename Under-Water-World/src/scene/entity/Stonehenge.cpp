#include <GL/glew.h> // Wymagane dla funkcji OpenGL na samym pocz¹tku!
#include "scene/entity/Stonehenge.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Stonehenge::Stonehenge() // Test
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = 1.0f;
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    pivotOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    shaderProgram = 0;
}

bool Stonehenge::init(
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
        std::cout << "Failed to create Stonehenge shader program" << std::endl;
        return false;
    }


    if (!model.loadFromFile(modelPath))
    {
        return false;
    }

    return true;
}

void Stonehenge::update(float deltaTime, const Input& input)
{
}

void Stonehenge::render(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    glUseProgram(shaderProgram);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::translate(modelMatrix, pivotOffset);

    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

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

    glm::vec3 cameraPos = glm::vec3(glm::inverse(view)[3]);
    glUniform3fv(
        glGetUniformLocation(shaderProgram, "cameraPos"),
        1,
        glm::value_ptr(cameraPos)
    );


    model.draw();
}

void Stonehenge::shutdown()
{

    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Stonehenge::getPosition() const
{
    return position;
}

void Stonehenge::setPosition(glm::vec3 newPosition)
{
    position = newPosition;
}

void Stonehenge::setScale(float newScale)
{
    scale = newScale;
}

void Stonehenge::setRotation(glm::vec3 newRotation)
{
    rotation = newRotation;
}

void Stonehenge::setPivotOffset(glm::vec3 offset)
{
    pivotOffset = offset;
}