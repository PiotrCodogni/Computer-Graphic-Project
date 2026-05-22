#pragma once

#include <GL/glew.h>
#include <iostream>
#include <string>

namespace Core
{
    class Shader_Loader
    {
    private:
        std::string ReadShader(const char* filename);

        GLuint CreateShader(
            GLenum shaderType,
            std::string source,
            const char* shaderName
        );

    public:
        Shader_Loader(void);
        ~Shader_Loader(void);

        GLuint CreateProgram(
            const char* vertexShaderFilename,
            const char* fragmentShaderFilename
        );

        void DeleteProgram(GLuint program);
    };
}