#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 cameraPos;

void main()
{
    vec4 texColor = texture(colorTexture, texCoord);
    
    // Discard transparent pixels using alpha channel from PNG
    if (texColor.a < 0.15)
    {
        discard;
    }
    
    vec3 resultColor = texColor.rgb;
    
    // Underwater Fog
    float distance = length(cameraPos - fragPos);
    vec3 fogColor = vec3(0.05, 0.35, 0.55);
    float fogDensity = 0.025;
    float fogFactor = exp(-distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 finalColor = mix(fogColor, resultColor, fogFactor);
    
    FragColor = vec4(finalColor, 1.0);
}
