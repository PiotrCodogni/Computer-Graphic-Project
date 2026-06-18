#version 330 core

in vec3 texCoord;

out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    vec3 color = texture(skybox, texCoord).rgb;
    FragColor = vec4(color, 1.0);
}
