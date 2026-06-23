#pragma once
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct PTFrame {
    glm::vec3 P;
    glm::vec3 T;
    glm::vec3 N;
    glm::vec3 B;
};

class SchoolOfFish {
public:
    SchoolOfFish();
    bool init(const char* modelPath, const char* texturePath, const char* vShader, const char* fShader);
    void update(glm::vec3 targetPosition, float deltaTime);
    void render(const glm::mat4& view,
        const glm::mat4& proj, 
        const glm::vec3& camPos, 
        const glm::vec3& lightPos, 
        const glm::vec3& fogColor, 
        float fogDensity,
        const glm::mat4& lightSpaceMatrix,
        GLuint shadowMap,
        bool useShadows);
    void renderDepth(GLuint depthShader, const glm::mat4& lightSpaceMatrix);
    void shutdown();
    float animationTime = 0.0f;

private:
    Model model;
    Texture texture;
    float currentProgress;
    GLuint shaderProgram;
    std::vector<float> fishProgress;
    Core::Shader_Loader shaderLoader;

    std::vector<glm::vec3> pathHistory;
    std::vector<PTFrame> frames;

    void updateFrames();
    glm::mat4 getFishModelMatrix(const PTFrame& frame) const;
};
