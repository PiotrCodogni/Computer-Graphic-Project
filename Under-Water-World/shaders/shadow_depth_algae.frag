#version 330 core

in vec2 texCoord;

uniform sampler2D alphaTexture;

void main()
{
    if (texture(alphaTexture, texCoord).a < 0.15)
    {
        discard;
    }
}
