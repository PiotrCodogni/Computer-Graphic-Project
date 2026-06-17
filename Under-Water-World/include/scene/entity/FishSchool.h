#pragma once

#include "graphics/Shader.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct SchoolFishInstance
{
    glm::vec3 position;
    float     heading;       // kat obrotu wokol Y (radiany)
    float     speed;         // indywidualna predkosc
    float     phaseOffset;   // przesuniecie fazy animacji ogona
    float     orbitRadius;   // promien orbity wokol centrum lawicy
    float     orbitAngle;    // aktualny kat na orbicie
    float     orbitSpeed;    // predkosc poruszania sie po orbicie
    float     heightOffset;  // przesuniecie wysokosci wzgledem centrum
    float     scale;         // skala rybki (lekka roznorodnosc)
};

class FishSchool
{
public:
    FishSchool();

    bool init(
        const char* modelPath,
        const char* texturePath,
        const char* vertexShaderPath,
        const char* fragmentShaderPath,
        int fishCount,
        glm::vec3 centerPosition
    );

    void update(float deltaTime);

    void render(const glm::mat4& view,
        const glm::mat4& projection,
        const glm::vec3& cameraPos, 
        const glm::vec3& fogColor, 
        float fogDensity, 
        const glm::vec3& lightPos);

    void shutdown();

private:
    std::vector<SchoolFishInstance> fishList;

    glm::vec3 schoolCenter;  // punkt centralny lawicy
    float     schoolTime;    // czas animacji

    GLuint shaderProgram;

    Core::Shader_Loader shaderLoader;

    Model   model;
    Texture texture;
};
