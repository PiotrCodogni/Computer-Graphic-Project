#pragma once
#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include <vector>
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
    void update(glm::vec3 targetPosition);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void shutdown();

private:
    Model model;
    Texture texture;
    GLuint shaderProgram;
    Core::Shader_Loader shaderLoader;

    std::vector<glm::vec3> pathHistory;
    std::vector<PTFrame> frames;

    void updateFrames();
};