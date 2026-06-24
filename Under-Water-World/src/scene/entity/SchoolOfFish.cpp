#include "scene/entity/SchoolOfFish.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <algorithm>

SchoolOfFish::SchoolOfFish() : shaderProgram(0), animationTime(0.0f), currentProgress(0.0f) {}

bool SchoolOfFish::init(const char* mPath, const char* tPath, const char* vShader, const char* fShader) {
    shaderProgram = shaderLoader.CreateProgram(vShader, fShader);
    if (shaderProgram == 0) return false;
    if (!texture.loadFromFile(tPath, true)) return false;
    if (!model.loadFromFile(mPath)) return false;

    // ------------------------------------------------------------------
    // GENEROWANIE ZAMKNI�TEJ P�TLI SPLAJNU
    // ------------------------------------------------------------------
    std::vector<glm::vec3> controlPoints = {
        glm::vec3(-36.33f, -0.36f, -55.9f),
        glm::vec3(-24.12f, 5.66f, -56.76f),
        glm::vec3(-14.26f, 9.15f, -62.35f),
        glm::vec3(-4.02f, 16.12f, -70.16f),
        glm::vec3(2.36f, 19.52f, -60.94f),
        glm::vec3(9.38f, 10.82f, -52.71f),
        glm::vec3(14.78f, 1.30f, -61.95f),
        glm::vec3(-1.70f, 0.97f, -71.69f),
        glm::vec3(-30.32f, 0.65f, -71.87f)
    };

    std::vector<glm::vec3> loopPositions;
    size_t N = controlPoints.size();
    int pointsPerSegment = 120;

    for (size_t i = 0; i < N; ++i) {
        glm::vec3 p0 = controlPoints[(i - 1 + N) % N];
        glm::vec3 p1 = controlPoints[i];
        glm::vec3 p2 = controlPoints[(i + 1) % N];
        glm::vec3 p3 = controlPoints[(i + 2) % N];

        for (int step = 0; step < pointsPerSegment; ++step) {
            float t = (float)step / (float)pointsPerSegment;

            // Interpolacja Catmull-Rom
            glm::vec3 pos = 0.5f * ((2.0f * p1) +
                (-p0 + p2) * t +
                (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
                (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
            loopPositions.push_back(pos);
        }
    }

    // ------------------------------------------------------------------
    // PARALLEL TRANSPORT FRAMES PRECOMPUTATION
    // ------------------------------------------------------------------
    size_t M = loopPositions.size();
    frames.resize(M);

    for (size_t i = 0; i < M; ++i) {
        frames[i].P = loopPositions[i];
        glm::vec3 nextP = loopPositions[(i + 1) % M];
        frames[i].T = glm::normalize(nextP - loopPositions[i]);
    }

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (std::abs(glm::dot(frames[0].T, up)) > 0.99f) {
        up = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    frames[0].B = glm::normalize(glm::cross(frames[0].T, up));
    frames[0].N = glm::cross(frames[0].B, frames[0].T);

    fishProgress.resize(5, 0.0f); // 5 rybek
    for (int i = 0; i < 5; ++i) {
        fishProgress[i] = (float)frames.size() - (i * 60.0f); // Pocz�tkowy odst�p
    }

    for (size_t i = 1; i < M; ++i) {
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

    return true;
}

void SchoolOfFish::update(glm::vec3 targetPos, float dt) {
    animationTime += dt;
    float speed = 65.0f;
    for (float& p : fishProgress) {
        p += speed * dt;
        if (p >= (float)frames.size()) p -= (float)frames.size();
    }
}

void SchoolOfFish::render(
    const glm::mat4& view,
    const glm::mat4& proj,
    const glm::vec3& camPos,
    const glm::vec3& lightPos,
    const glm::vec3& fogColor,
    float fogDensity,
    const glm::mat4& lightSpaceMatrix,
    GLuint shadowMap,
    bool useShadows
) {
    if (frames.size() < 2) return;

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensity);
    glUniform1i(glGetUniformLocation(shaderProgram, "useShadows"), useShadows ? 1 : 0);

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 5);

    for (size_t i = 0; i < fishProgress.size(); ++i) {
        int idx = (int)fishProgress[i] % (int)frames.size();
        const PTFrame& f = frames[idx];
        glm::mat4 m_mat = getFishModelMatrix(f);

        glUniform1f(glGetUniformLocation(shaderProgram, "time"), animationTime + static_cast<float>(i) * 0.35f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(m_mat));
        model.draw();
    }
}

void SchoolOfFish::renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix)
{
    if (frames.size() < 2) return;

    glUseProgram(depthShader);
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    for (size_t i = 0; i < fishProgress.size(); ++i) {
        int idx = (int)fishProgress[i] % (int)frames.size();
        const PTFrame& f = frames[idx];
        glm::mat4 m_mat = getFishModelMatrix(f);

        glUniform1f(glGetUniformLocation(depthShader, "time"), animationTime + static_cast<float>(i) * 0.35f);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(m_mat));
        model.drawDepth();
    }
}

glm::mat4 SchoolOfFish::getFishModelMatrix(const PTFrame& frame) const
{
    glm::vec3 T = frame.T;
    glm::vec3 w_up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (std::abs(glm::dot(T, w_up)) > 0.99f) w_up = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 B = glm::normalize(glm::cross(w_up, T));
    glm::vec3 N = glm::normalize(glm::cross(T, B));

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), frame.P);

    glm::mat4 rotMat(1.0f);
    rotMat[0] = glm::vec4(B, 0.0f);
    rotMat[1] = glm::vec4(N, 0.0f);
    rotMat[2] = glm::vec4(T, 0.0f);

    modelMatrix = modelMatrix * rotMat;
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = modelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(15.0f));

    return modelMatrix;
}

void SchoolOfFish::shutdown() {
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}
