#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

out vec3 fragPos;
out vec3 normalWorld;
out vec2 texCoord;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec4 worldPosition = model * vec4(vertexPosition, 1.0);

    fragPos = worldPosition.xyz;
    normalWorld = mat3(transpose(inverse(model))) * vertexNormal;
    texCoord = vertexTexCoord;
    fragPosLightSpace = lightSpaceMatrix * worldPosition;

    gl_Position = projection * view * worldPosition;
}
