#include "scene/entity/Fish1.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <iostream>

Fish::Fish()
{
    position = glm::vec3(-12.0f, 0.0f, -18.0f);

    rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    rotationSpeed = 6.0f;

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

	// obliczanie kierunku ruchu na podstawie wciśniętych klawiszy
    glm::vec3 moveDirection(0.0f);

    if (input.isKeyPressed(GLFW_KEY_W))
    {
        moveDirection.z -= 1.0f;
    }

    if (input.isKeyPressed(GLFW_KEY_S))
    {
        moveDirection.z += 1.0f;
    }

    if (input.isKeyPressed(GLFW_KEY_A))
    {
        moveDirection.x -= 1.0f;
    }

    if (input.isKeyPressed(GLFW_KEY_D))
    {
        moveDirection.x += 1.0f;
    }

    if (input.isKeyPressed(GLFW_KEY_SPACE))
    {
        moveDirection.y += 1.0f;
    }

    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        moveDirection.y -= 1.0f;
    }

	// normalizacja kierunku ruchu i aktualizacja pozycji
    if (glm::length(moveDirection) > 0.0f)
    {
        moveDirection = glm::normalize(moveDirection);

        position += moveDirection * speed * deltaTime;

        glm::vec3 horizontalDirection = glm::vec3(
            moveDirection.x,
            0.0f,
            moveDirection.z
        );

		// obracanie ryby w kierunku ruchu
        if (glm::length(horizontalDirection) > 0.0f)
        {
            horizontalDirection = glm::normalize(horizontalDirection);

            float targetYaw = std::atan2(
                -horizontalDirection.x,
                -horizontalDirection.z
            );
            
            glm::quat targetRotation = glm::angleAxis(
                targetYaw,
                glm::vec3(0.0f, 1.0f, 0.0f)
            );
			// interpolacja rotacji ryby w kierunku docelowej rotacji
            float t = glm::clamp(rotationSpeed * deltaTime, 0.0f, 1.0f);
            rotation = glm::slerp(rotation, targetRotation, t);
        }
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
    modelMatrix *= glm::mat4_cast(rotation);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    modelMatrix = glm::rotate(
        modelMatrix,
        glm::radians(-90.0f),
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

    glm::vec3 cameraPos = glm::vec3(glm::inverse(view)[3]);
    glUniform3fv(
        glGetUniformLocation(shaderProgram, "cameraPos"),
        1,
        glm::value_ptr(cameraPos)
    );

    glm::vec3 fogColor = glm::vec3(0.05f, 0.35f, 0.55f);
    float fogDensity = 0.025f;
    glUniform3fv(
        glGetUniformLocation(shaderProgram, "fogColor"),
        1,
        glm::value_ptr(fogColor)
    );
    glUniform1f(
        glGetUniformLocation(shaderProgram, "fogDensity"),
        fogDensity
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
