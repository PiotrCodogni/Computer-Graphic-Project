#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

class Cubemap
{
public:
    Cubemap();

    bool loadFromFiles(const std::vector<std::string>& faces);
    void bind(unsigned int slot) const;
    void shutdown();

    GLuint getId() const;

private:
    GLuint textureId;
};