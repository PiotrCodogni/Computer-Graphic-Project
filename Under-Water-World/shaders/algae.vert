#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;

void main()
{
    vec3 pos = vertexPosition;
    
    // Wave animation: only affect parts above ground (y > 0)
    // Scale animation with height so top waves more than bottom
    float wave = sin(time * 2.2 + pos.y * 1.0) * 0.18 * max(0.0, pos.y);
    pos.x += wave;
    pos.z += wave * 0.4;
    
    fragPos = vec3(model * vec4(pos, 1.0));
    fragNormal = mat3(transpose(inverse(model))) * vertexNormal;
    texCoord = vertexTexCoord;
    gl_Position = projection * view * vec4(fragPos, 1.0);
}
