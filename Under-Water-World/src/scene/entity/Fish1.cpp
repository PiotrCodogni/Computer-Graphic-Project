#include "scene/entity/Fish1.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

Fish::Fish() {
    position = glm::vec3(-12.0f, 0.0f, -18.0f);
    rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    rotationSpeed = 6.0f;
    speed = 2.0f;
    scale = 40.0f;
    animationTime = 0.0f;
    shaderProgram = 0;
}

bool Fish::init(const char* modPath, const char* texPath, const char* vShader, const char* fShader) {
    shaderProgram = shaderLoader.CreateProgram(vShader, fShader);
    if (shaderProgram == 0) return false;
    if (!texture.loadFromFile(texPath, true)) return false;
    if (!model.loadFromFile(modPath)) return false;
    return true;
}
void Fish::update(float deltaTime, const Input& input) {
    // Wywo�ujemy drug� wersj� z domy�lnym wektorem patrzenia (np. w prz�d osi Z)
    update(deltaTime, input, glm::vec3(0.0f, 0.0f, -1.0f));
}

void Fish::update(float deltaTime, const Input& input, const glm::vec3& cameraForward) {
    animationTime += deltaTime;

    glm::vec3 forward = cameraForward;
    glm::vec3 right = glm::vec3(-forward.z, 0.0f, forward.x);
    glm::vec3 moveDir(0.0f);

    if (input.isKeyPressed(GLFW_KEY_W)) moveDir += forward;
    if (input.isKeyPressed(GLFW_KEY_S)) moveDir -= forward;
    if (input.isKeyPressed(GLFW_KEY_A)) moveDir -= right;
    if (input.isKeyPressed(GLFW_KEY_D)) moveDir += right;

    if (input.isKeyPressed(GLFW_KEY_SPACE)) moveDir.y += 1.0f;
    if (input.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) moveDir.y -= 1.0f;

    if (glm::length(moveDir) > 0.001f) {
        moveDir = glm::normalize(moveDir);
        position += moveDir * speed * deltaTime;

        glm::vec3 horizDir = glm::vec3(moveDir.x, 0.0f, moveDir.z);
        if (glm::length(horizDir) > 0.001f) {
            horizDir = glm::normalize(horizDir);
            float tgtYaw = std::atan2(-horizDir.x, -horizDir.z);
            glm::quat tgtRot = glm::angleAxis(tgtYaw, glm::vec3(0.0f, 1.0f, 0.0f));
            float t = glm::clamp(rotationSpeed * deltaTime, 0.0f, 1.0f);
            rotation = glm::slerp(rotation, tgtRot, t);
        }
    }
}

void Fish::render(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& lightPos,
    const glm::vec3& fogColor,
    float fogDensity,
    const glm::mat4& lightSpaceMatrix,
    GLuint shadowMap,
    bool useShadows
)
{
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), animationTime);

    glm::mat4 modelMatrix = getModelMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);
    glUniform1i(glGetUniformLocation(shaderProgram, "useShadows"), useShadows ? 1 : 0);

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 5);

    model.draw();
}

void Fish::renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix)
{
    glUseProgram(depthShader);

    glm::mat4 modelMatrix = getModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform1f(glGetUniformLocation(depthShader, "time"), animationTime);

    model.drawDepth();
}

glm::mat4 Fish::getModelMatrix() const
{
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

    return modelMatrix;
}

void Fish::shutdown()
{
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Fish::getPosition() const {
    return position;
}
