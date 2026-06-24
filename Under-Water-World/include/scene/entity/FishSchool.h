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

    // Bazowe wartosci do interpolacji miedzy trybami
    float     baseOrbitSpeed;
    float     baseOrbitRadius;

    // Ucieczka - kazda ryba ucieka w swoim losowym kierunku
    glm::vec3 escapeDir;     // znormalizowany kierunek ucieczki
    glm::vec3 escapeOffset;  // aktualne przesuniecie od pozycji orbitowej
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
        const glm::vec3& lightPos,
        const glm::mat4& lightSpaceMatrix,
        GLuint shadowMap,
        bool useShadows
    );

    void renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix);

    void shutdown();

    void togglePanicMode();
    bool isPanicking() const { return panicMode; }

private:
    glm::mat4 getFishModelMatrix(const SchoolFishInstance& fish) const;

    std::vector<SchoolFishInstance> fishList;

    glm::vec3 schoolCenter;  // punkt centralny lawicy
    float     schoolTime;    // czas animacji

    bool      panicMode;     // tryb paniki (F)
    float     panicBlend;    // plynne przejscie 0.0 (spokojnie) -> 1.0 (panika)

    GLuint shaderProgram;

    Core::Shader_Loader shaderLoader;

    Model   model;
    Texture texture;
};
