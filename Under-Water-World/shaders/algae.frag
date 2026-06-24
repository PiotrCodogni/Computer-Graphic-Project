#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;
in vec4 fragPosLightSpace;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D shadowMap;
uniform vec3 cameraPos;
uniform vec3 fogColor;
uniform float fogDensity;
uniform bool useShadows;

float ShadowCalculation(vec4 lightSpacePos, vec3 normal)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    {
        return 0.0;
    }

    vec3 lightDir = normalize(vec3(-200.19, 290.01, -24.38) - fragPos);
    float bias = max(0.005 * (1.0 - dot(normalize(normal), lightDir)), 0.0005);
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    return shadow / 9.0;
}

void main()
{
    vec4 texColor = texture(colorTexture, texCoord);
    
    // Discard transparent pixels using alpha channel from PNG
    if (texColor.a < 0.15)
    {
        discard;
    }
    
    float shadow = useShadows ? ShadowCalculation(fragPosLightSpace, fragNormal) : 0.0;
    vec3 resultColor = texColor.rgb * (1.0 - shadow * 0.5);
    
    // Underwater Fog
    float distance = length(cameraPos - fragPos);
    float fogFactor = exp(-distance * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    
    vec3 finalColor = mix(fogColor, resultColor, fogFactor);
    
    FragColor = vec4(finalColor, 1.0);
}
