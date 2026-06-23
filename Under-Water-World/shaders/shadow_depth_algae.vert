#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform float time;

out vec2 texCoord;

void main()
{
    vec3 pos = vertexPosition;
    float wave = sin(time * 2.2 + pos.y * 1.0) * 0.18 * max(0.0, pos.y);
    pos.x += wave;
    pos.z += wave * 0.4;

    texCoord = vertexTexCoord;
    gl_Position = lightSpaceMatrix * model * vec4(pos, 1.0);
}
