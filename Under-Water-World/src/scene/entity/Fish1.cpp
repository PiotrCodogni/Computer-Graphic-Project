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
    // Wywo³ujemy drug¹ wersjê z domyœlnym wektorem patrzenia (np. w przód osi Z)
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

void Fish::render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos, const glm::vec3& lightPos, const glm::vec3& fogColor, float fogDensity) {
    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), animationTime);

    glm::mat4 modMat = glm::mat4(1.0f);
    modMat = glm::translate(modMat, position);
    modMat *= glm::mat4_cast(rotation);
    modMat = glm::scale(modMat, glm::vec3(scale));
    modMat = glm::rotate(modMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modMat = glm::rotate(modMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modMat));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    model.draw();
}

void Fish::shutdown() {
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Fish::getPosition() const {
    return position;
}