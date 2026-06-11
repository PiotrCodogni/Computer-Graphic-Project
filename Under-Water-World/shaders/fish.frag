#version 330 core

in vec2 texCoord;
in vec3 fragPos;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 cameraPos;
uniform vec3 fogColor;
uniform float fogDensity;

void main()
{
    vec3 resultColor = texture(colorTexture, texCoord).rgb;
    
    // Underwater Fog
    float distance = length(cameraPos - fragPos);
    float fogFactor = exp(-distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 finalColor = mix(fogColor, resultColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}