#include "scene/entity/FishSchool.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <cstdlib>
#include <cmath>

FishSchool::FishSchool() {
    schoolCenter = glm::vec3(0.0f);
    schoolTime = 0.0f;
    shaderProgram = 0;
}

bool FishSchool::init(const char* modPath, const char* texPath, const char* vShader, const char* fShader, int fCount, glm::vec3 centPos) {
    schoolCenter = centPos;

    shaderProgram = shaderLoader.CreateProgram(vShader, fShader);
    if (shaderProgram == 0) return false;
    if (!texture.loadFromFile(texPath, true)) return false;
    if (!model.loadFromFile(modPath)) return false;

    std::srand(54321);
    for (int i = 0; i < fCount; ++i) {
        SchoolFishInstance f;

        float rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.orbitRadius = 3.0f + rFlt * 8.0f;

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.orbitAngle = rFlt * glm::two_pi<float>();

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.orbitSpeed = 0.15f + rFlt * 0.35f;

        if (std::rand() % 2 == 0) f.orbitSpeed = -f.orbitSpeed;

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.heightOffset = -2.0f + rFlt * 4.0f;

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.phaseOffset = rFlt * glm::two_pi<float>();

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.speed = 1.0f + rFlt * 1.5f;

        rFlt = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        f.scale = 25.0f + rFlt * 20.0f;

        f.position = schoolCenter + glm::vec3(f.orbitRadius * std::cos(f.orbitAngle), f.heightOffset, f.orbitRadius * std::sin(f.orbitAngle));
        f.heading = 0.0f;

        fishList.push_back(f);
    }
    return true;
}

void FishSchool::update(float dt) {
    schoolTime += dt;

    glm::vec3 dCent = schoolCenter;
    dCent.x += std::sin(schoolTime * 0.05f) * 3.0f;
    dCent.z += std::cos(schoolTime * 0.07f) * 3.0f;
    dCent.y += std::sin(schoolTime * 0.1f) * 0.5f;

    for (auto& f : fishList) {
        f.orbitAngle += f.orbitSpeed * dt;

        float cRad = f.orbitRadius + std::sin(schoolTime * 0.3f + f.phaseOffset) * 1.5f;

        glm::vec3 nPos;
        nPos.x = dCent.x + cRad * std::cos(f.orbitAngle);
        nPos.z = dCent.z + cRad * std::sin(f.orbitAngle);
        nPos.y = dCent.y + f.heightOffset + std::sin(schoolTime * 0.5f + f.phaseOffset) * 0.5f;

        glm::vec3 mDir = nPos - f.position;
        if (glm::length(mDir) > 0.001f) {
            f.heading = std::atan2(mDir.x, mDir.z) + glm::pi<float>();
        }

        f.position = nPos;
    }
}

void FishSchool::render(const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos, const glm::vec3& fCol, float fDens, const glm::vec3& lightPos) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(camPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fCol));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fDens);

    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    for (const auto& f : fishList) {
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), schoolTime + f.phaseOffset);

        glm::mat4 modMat = glm::mat4(1.0f);
        modMat = glm::translate(modMat, f.position);
        modMat = glm::rotate(modMat, f.heading, glm::vec3(0.0f, 1.0f, 0.0f));
        modMat = glm::scale(modMat, glm::vec3(f.scale));
        modMat = glm::rotate(modMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modMat = glm::rotate(modMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modMat));

        model.draw();
    }
}

void FishSchool::shutdown() {
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}