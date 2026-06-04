#include "graphics/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>

Texture::Texture()
{
    textureId = 0;
}

bool Texture::loadFromFile(const char* path, bool flipVertically)
{
    int width;
    int height;
    int channels;

    stbi_set_flip_vertically_on_load(flipVertically);

    // Force stb_image to load 4 channels (RGBA) for consistency and transparency support
    unsigned char* data = stbi_load(
        path,
        &width,
        &height,
        &channels,
        4
    );

    if (data == nullptr)
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        return false;
    }

    GLenum format = GL_RGBA;

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    std::cout << "Loaded texture: " << path << std::endl;

    return true;
}

void Texture::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureId);
}

void Texture::shutdown()
{
    if (textureId != 0)
    {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
}

GLuint Texture::getId() const
{
    return textureId;
}