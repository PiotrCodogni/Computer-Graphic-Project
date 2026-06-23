#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform float time;

out vec2 texCoord;
out vec3 fragPos;
out vec3 fragNormal;
out vec4 fragPosLightSpace;

void main()
{
    vec3 pos = vertexPosition;

    float tailMask = smoothstep(0.0, 0.02, -pos.x);

    float wave = sin(time * 8.0 + pos.x * 120.0);

    pos.y += wave * 0.004 * tailMask;

    texCoord = vertexTexCoord;
    fragPos = vec3(model * vec4(pos, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * vertexNormal;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    gl_Position = projection * view * vec4(fragPos, 1.0);
}
