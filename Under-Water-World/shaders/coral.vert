#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;
out vec4 fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    texCoord = aTexCoords;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    gl_Position = projection * view * vec4(fragPos, 1.0);
}