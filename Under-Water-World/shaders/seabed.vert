#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;
out vec4 fragPosLightSpace;

void main()
{
    fragPos = vec3(model * vec4(vertexPosition, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * vertexNormal;
    texCoord = vertexTexCoord;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    gl_Position = projection * view * vec4(fragPos, 1.0);
}
