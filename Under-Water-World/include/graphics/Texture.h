#pragma once

#include <GL/glew.h>

class Texture
{
public:
    Texture();

    bool loadFromFile(const char* path, bool flipVertically);
    void bind(unsigned int slot) const;
    void shutdown();

    GLuint getId() const;

private:
    GLuint textureId;
};