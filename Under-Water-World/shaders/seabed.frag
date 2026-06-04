#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
    // Ambient light
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * vec3(0.8, 0.9, 1.0); // Slightly tinted blue ambient
    
    // Diffuse light (simple sunlight from top)
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    // Combine lighting
    vec3 texColor = texture(colorTexture, texCoord).rgb;
    vec3 resultColor = (ambient + diffuse) * texColor;
    
    // Underwater Fog (mix with water clear color based on distance to camera)
    float distance = length(cameraPos - fragPos);
    
    // Water color: (0.0, 0.25, 0.45)
    vec3 fogColor = vec3(0.0, 0.25, 0.45);
    
    // Exponential fog
    float fogDensity = 0.025;
    float fogFactor = exp(-distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 finalColor = mix(fogColor, resultColor, fogFactor);
    
    FragColor = vec4(finalColor, 1.0);
}
