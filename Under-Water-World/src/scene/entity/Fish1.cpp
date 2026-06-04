#include "scene/entity/Fish1.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

Fish::Fish()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    speed = 2.0f;
    scale = 40.0f;
    animationTime = 0.0f;

    shaderProgram = 0;
}

bool Fish::init(
    const char* modelPath,
    const char* texturePath,
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
        std::cout << "Failed to create fish shader program" << std::endl;
        return false;
    }

    if (!texture.loadFromFile(texturePath, true))
    {
        return false;
    }

    if (!model.loadFromFile(modelPath))
    {
        return false;
    }

    return true;
}

void Fish::update(float deltaTime, const Input& input)
{   
    animationTime += deltaTime;
    float movement = speed * deltaTime;
    

    if (input.isKeyPressed(GLFW_KEY_W))
    {
        position.z -= movement;
    }

    if (input.isKeyPressed(GLFW_KEY_S))
    {
        position.z += movement;
    }

    if (input.isKeyPressed(GLFW_KEY_A))
    {
        position.x -= movement;
    }

    if (input.isKeyPressed(GLFW_KEY_D))
    {
        position.x += movement;
    }

    if (input.isKeyPressed(GLFW_KEY_SPACE))
    {
        position.y += movement;
    }

    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        position.y -= movement;
    }
}

void Fish::render(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    glUseProgram(shaderProgram);

    glUniform1f(
        glGetUniformLocation(shaderProgram, "time"),
        animationTime
    );

    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(90.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

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

    texture.bind(0);

    glUniform1i(
        glGetUniformLocation(shaderProgram, "colorTexture"),
        0
    );



    model.draw();
}

void Fish::shutdown()
{
    texture.shutdown();

    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Fish::getPosition() const
{
    return position;
}