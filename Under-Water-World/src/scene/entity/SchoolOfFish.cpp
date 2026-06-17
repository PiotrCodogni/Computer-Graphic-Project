#include "scene/entity/SchoolOfFish.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>

SchoolOfFish::SchoolOfFish() : shaderProgram(0), animationTime(0.0f) {}

bool SchoolOfFish::init(const char* mPath, const char* tPath, const char* vShader, const char* fShader) {
    shaderProgram = shaderLoader.CreateProgram(vShader, fShader);
    if (shaderProgram == 0) return false;
    if (!texture.loadFromFile(tPath, true)) return false;
    if (!model.loadFromFile(mPath)) return false;
    return true;
}

void SchoolOfFish::update(glm::vec3 targetPos, float dt) {
    animationTime += dt;

    if (pathHistory.empty() || glm::distance(pathHistory.back(), targetPos) > 0.05f) {
        pathHistory.push_back(targetPos);

        if (pathHistory.size() > 400) {
            pathHistory.erase(pathHistory.begin());
        }
        updateFrames();
    }
}

void SchoolOfFish::updateFrames() {
    frames.clear();
    if (pathHistory.size() < 2) return;

    frames.resize(pathHistory.size());
    frames[0].P = pathHistory[0];
    frames[0].T = glm::normalize(pathHistory[1] - pathHistory[0]);
    glm::vec3 up = (std::abs(frames[0].T.y) > 0.99f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
    frames[0].B = glm::normalize(glm::cross(frames[0].T, up));
    frames[0].N = glm::cross(frames[0].B, frames[0].T);

    for (size_t i = 1; i < pathHistory.size(); ++i) {
        frames[i].P = pathHistory[i];

        if (i < pathHistory.size() - 1) {
            frames[i].T = glm::normalize(pathHistory[i + 1] - pathHistory[i]);
        }
        else {
            frames[i].T = frames[i - 1].T;
        }

        glm::vec3 axis = glm::cross(frames[i - 1].T, frames[i].T);
        float dotP = glm::clamp(glm::dot(frames[i - 1].T, frames[i].T), -1.0f, 1.0f);

        if (glm::length(axis) < 0.0001f) {
            frames[i].N = frames[i - 1].N;
        }
        else {
            float angle = std::acos(dotP);
            axis = glm::normalize(axis);
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, axis);
            frames[i].N = glm::normalize(glm::vec3(rot * glm::vec4(frames[i - 1].N, 0.0f)));
        }
        frames[i].B = glm::normalize(glm::cross(frames[i].T, frames[i].N));
    }
}

void SchoolOfFish::render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos, const glm::vec3& lightPos, const glm::vec3& fogColor, float fogDensity) {
    if (frames.size() < 2) return;

    glUseProgram(shaderProgram);
    glUniform1f(glGetUniformLocation(shaderProgram, "time"), animationTime);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    std::vector<glm::vec3> baseOff = {
        glm::vec3(-1.5f,  0.0f, 0.0f),
        glm::vec3(1.5f,  0.5f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(-2.5f, -0.5f, 0.0f),
        glm::vec3(2.5f,  0.0f, 0.0f)
    };

    for (size_t i = 0; i < baseOff.size(); ++i) {
        float timeVal = animationTime * 2.0f + i * 1.5f;
        int lag = (int)((std::sin(timeVal) + 1.0f) * 12.0f);

        int idx = std::max(0, (int)frames.size() - 1 - (int)(i * 30 + 40) - lag);
        if (idx >= frames.size()) continue;

        PTFrame& f = frames[idx];

        glm::vec3 drift = glm::vec3(
            std::sin(animationTime * 0.8f + i) * 0.8f,
            std::cos(animationTime * 1.1f + i * 2.0f) * 0.6f,
            0.0f
        );

        glm::mat4 modelMat(1.0f);
        modelMat[0] = glm::vec4(f.B, 0.0f);
        modelMat[1] = glm::vec4(f.N, 0.0f);
        modelMat[2] = glm::vec4(f.T, 0.0f);
        modelMat[3] = glm::vec4(f.P, 1.0f);

        modelMat = glm::translate(modelMat, baseOff[i] + drift);
        modelMat = glm::scale(modelMat, glm::vec3(15.0f));

        modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
        model.draw();
    }
}

void SchoolOfFish::shutdown() {
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}