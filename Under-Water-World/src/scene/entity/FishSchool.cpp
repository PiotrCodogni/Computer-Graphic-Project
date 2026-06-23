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

void FishSchool::render(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& fogColorVal,
    float fogDensityVal,
    const glm::vec3& lightPos,
    const glm::mat4& lightSpaceMatrix,
    GLuint shadowMap,
    bool useShadows
)
{
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shaderProgram, "cameraPos"), 1, glm::value_ptr(cameraPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "fogColor"), 1, glm::value_ptr(fogColorVal));
    glUniform1f(glGetUniformLocation(shaderProgram, "fogDensity"), fogDensityVal);
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform1i(glGetUniformLocation(shaderProgram, "useShadows"), useShadows ? 1 : 0);

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 5);

    for (const auto& fish : fishList)
    {
        // Czas animacji ogona - kazda rybka ma inne przesuniecie fazy
        glUniform1f(
            glGetUniformLocation(shaderProgram, "time"),
            schoolTime + fish.phaseOffset
        );

        glm::mat4 modelMatrix = getFishModelMatrix(fish);

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(modelMatrix)
        );

        model.draw();
    }
}

void FishSchool::renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix)
{
    glUseProgram(depthShader);
    glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    for (const auto& fish : fishList)
    {
        glUniform1f(glGetUniformLocation(depthShader, "time"), schoolTime + fish.phaseOffset);

        glm::mat4 modelMatrix = getFishModelMatrix(fish);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        model.drawDepth();
    }
}

glm::mat4 FishSchool::getFishModelMatrix(const SchoolFishInstance& fish) const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, fish.position);

    // Obrot w kierunku ruchu
    modelMatrix = glm::rotate(
        modelMatrix,
        fish.heading,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    modelMatrix = glm::scale(modelMatrix, glm::vec3(fish.scale));

    // Orientacja modelu (tak samo jak gracz)
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

void FishSchool::shutdown()
{
    texture.shutdown();
    if (shaderProgram != 0) {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}
