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

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;
out vec4 fragPosLightSpace;
out mat3 TBN;

void main()
{
    fragPos = vec3(model * vec4(vertexPosition, 1.0));

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    fragNormal = normalMatrix * vertexNormal;

    
    vec3 T = normalize(normalMatrix * vertexTangent);
    vec3 N = normalize(normalMatrix * vertexNormal);
    
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = mat3(T, B, N);

    texCoord = vertexTexCoord;
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    gl_Position = projection * view * vec4(fragPos, 1.0);
}
