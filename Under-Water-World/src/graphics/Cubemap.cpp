#include "graphics/Cubemap.h"

#include <stb_image.h>
#include <iostream>

Cubemap::Cubemap()
{
    textureId = 0;
}

bool Cubemap::loadFromFiles(const std::vector<std::string>& faces)
{
    if (faces.size() != 6)
    {
        std::cout << "Cubemap needs exactly 6 textures." << std::endl;
        return false;
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        int width;
        int height;
        int channels;

        unsigned char* data = stbi_load(
            faces[i].c_str(),
            &width,
            &height,
            &channels,
            4
        );

        if (!data)
        {
            std::cout << "Failed to load cubemap face: " << faces[i] << std::endl;
            stbi_image_free(data);
            shutdown();
            return false;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            data
        );

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "Loaded cubemap." << std::endl;
    return true;
}

void Cubemap::bind(unsigned int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}

void Cubemap::shutdown()
{
    if (textureId != 0)
    {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
}

GLuint Cubemap::getId() const
{
    return textureId;
}