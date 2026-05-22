#include "scene/Fish.h"

#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Fish::Fish()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    speed = 2.0f;
    scale = 40.0f;

    shaderProgram = 0;
    textureId = 0;
}

bool Fish::init(
    const char* modelPath,
    const char* texturePath,
    const char* vertexShaderPath,
    const char* fragmentShaderPath
)
{
    shaderProgram = shaderLoader.CreateProgram(
        vertexShaderPath,
        fragmentShaderPath
    );

    if (shaderProgram == 0)
    {
        std::cout << "Failed to create fish shader program" << std::endl;
        return false;
    }

    if (!loadTexture(texturePath))
    {
        return false;
    }

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (scene == nullptr || scene->mNumMeshes == 0)
    {
        std::cout << "Failed to load fish model: " << modelPath << std::endl;
        std::cout << importer.GetErrorString() << std::endl;
        return false;
    }


    renderContext.initFromAssimpMesh(scene->mMeshes[0]);

    return true;
}

bool Fish::loadTexture(const char* texturePath)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(
        texturePath,
        &width,
        &height,
        &channels,
        0
    );

    if (data == nullptr)
    {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
        return false;
    }

    GLenum format = GL_RGB;

    if (channels == 1)
    {
        format = GL_RED;
    }
    else if (channels == 3)
    {
        format = GL_RGB;
    }
    else if (channels == 4)
    {
        format = GL_RGBA;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    std::cout << "Loaded texture: " << texturePath << std::endl;

    return true;
}

void Fish::update(float deltaTime, const Input& input)
{
    float movement = speed * deltaTime;

    if (input.isKeyPressed(GLFW_KEY_W))
    {
        position.y += movement;
    }

    if (input.isKeyPressed(GLFW_KEY_S))
    {
        position.y -= movement;
    }

    if (input.isKeyPressed(GLFW_KEY_A))
    {
        position.x -= movement;
    }

    if (input.isKeyPressed(GLFW_KEY_D))
    {
        position.x += movement;
    }
}

void Fish::render(
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));

    model = glm::rotate(
        model,
        glm::radians(90.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    model = glm::rotate(
        model,
        glm::radians(90.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(model)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"),
        1,
        GL_FALSE,
        glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "projection"),
        1,
        GL_FALSE,
        glm::value_ptr(projection)
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glUniform1i(
        glGetUniformLocation(shaderProgram, "colorTexture"),
        0
    );

    Core::DrawContext(renderContext);
}

void Fish::shutdown()
{
    if (textureId != 0)
    {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }

    if (shaderProgram != 0)
    {
        shaderLoader.DeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
}

glm::vec3 Fish::getPosition() const
{
    return position;
}