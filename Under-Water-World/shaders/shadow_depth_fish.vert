#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform float time;

void main()
{
    vec3 pos = vertexPosition;
    float tailMask = smoothstep(0.0, 0.02, -pos.x);
    float wave = sin(time * 8.0 + pos.x * 120.0);
    pos.y += wave * 0.004 * tailMask;

    gl_Position = lightSpaceMatrix * model * vec4(pos, 1.0);
}
