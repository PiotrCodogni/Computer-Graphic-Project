#include "scene/entity/FishSchool.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>
#include <cstdlib>
#include <cmath>

FishSchool::FishSchool()
{
    schoolCenter = glm::vec3(0.0f);
    schoolTime   = 0.0f;
    shaderProgram = 0;
}

bool FishSchool::init(
    const char* modelPath,
    const char* texturePath,
    const char* vertexShaderPath,
    const char* fragmentShaderPath,
    int fishCount,
    glm::vec3 centerPosition
)
{
    schoolCenter = centerPosition;

    shaderProgram = shaderLoader.CreateProgram(vertexShaderPath, fragmentShaderPath);
    if (shaderProgram == 0)
    {
        std::cout << "Failed to create fish school shader" << std::endl;
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

    // Generowanie losowych rybek w lawicy
    std::srand(54321);
    for (int i = 0; i < fishCount; ++i)
    {
        SchoolFishInstance fish;

        // Kazda rybka krazy na swojej orbicie wokol centrum
        float randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.orbitRadius = 3.0f + randFloat * 8.0f;

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.orbitAngle = randFloat * glm::two_pi<float>();

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.orbitSpeed = 0.15f + randFloat * 0.35f;

        // Losowy kierunek orbity (polowa plynie w lewo, polowa w prawo)
        if (std::rand() % 2 == 0)
            fish.orbitSpeed = -fish.orbitSpeed;

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.heightOffset = -2.0f + randFloat * 4.0f;

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.phaseOffset = randFloat * glm::two_pi<float>();

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.speed = 1.0f + randFloat * 1.5f;

        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        fish.scale = 25.0f + randFloat * 20.0f;

        // Poczatkowa pozycja na orbicie
        fish.position = schoolCenter + glm::vec3(
            fish.orbitRadius * std::cos(fish.orbitAngle),
            fish.heightOffset,
            fish.orbitRadius * std::sin(fish.orbitAngle)
        );

        fish.heading = 0.0f;

        fishList.push_back(fish);
    }

    return true;
}

void FishSchool::update(float deltaTime)
{
    schoolTime += deltaTime;

    // Delikatny ruch centrum lawicy - lawica powoli dryfuje
    glm::vec3 driftCenter = schoolCenter;
    driftCenter.x += std::sin(schoolTime * 0.05f) * 3.0f;
    driftCenter.z += std::cos(schoolTime * 0.07f) * 3.0f;
    driftCenter.y += std::sin(schoolTime * 0.1f) * 0.5f;

    for (auto& fish : fishList)
    {
        // Ruch po orbicie
        fish.orbitAngle += fish.orbitSpeed * deltaTime;

        // Nowa pozycja na orbicie z lekkim falowaniem promienia
        float currentRadius = fish.orbitRadius + std::sin(schoolTime * 0.3f + fish.phaseOffset) * 1.5f;

        glm::vec3 newPos;
        newPos.x = driftCenter.x + currentRadius * std::cos(fish.orbitAngle);
        newPos.z = driftCenter.z + currentRadius * std::sin(fish.orbitAngle);
        newPos.y = driftCenter.y + fish.heightOffset + std::sin(schoolTime * 0.5f + fish.phaseOffset) * 0.5f;

        // Obliczanie kata obrotu - rybka patrzy w kierunku ruchu
        glm::vec3 moveDir = newPos - fish.position;
        if (glm::length(moveDir) > 0.001f)
        {
            fish.heading = std::atan2(moveDir.x, moveDir.z) + glm::pi<float>();
        }

        fish.position = newPos;
    }
}

void FishSchool::render(
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec3& cameraPos,
    const glm::vec3& fogColorVal,
    float fogDensityVal
)
{
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"),
        1, GL_FALSE, glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "projection"),
        1, GL_FALSE, glm::value_ptr(projection)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "cameraPos"),
        1, glm::value_ptr(cameraPos)
    );

    glUniform3fv(
        glGetUniformLocation(shaderProgram, "fogColor"),
        1, glm::value_ptr(fogColorVal)
    );

    glUniform1f(
        glGetUniformLocation(shaderProgram, "fogDensity"),
        fogDensityVal
    );

    texture.bind(0);
    glUniform1i(glGetUniformLocation(shaderProgram, "colorTexture"), 0);

    for (const auto& fish : fishList)
    {
        // Czas animacji ogona - kazda rybka ma inne przesuniecie fazy
        glUniform1f(
            glGetUniformLocation(shaderProgram, "time"),
            schoolTime + fish.phaseOffset
        );

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
            glm::radians(90.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(modelMatrix)
        );

        model.draw();
    }
}

void FishSchool::shutdown()
{
    texture.shutdown();

    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}
