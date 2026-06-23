#version 330 core
out vec4 FragColor;

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;
in vec4 fragPosLightSpace;

uniform sampler2D colorTexture;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 fogColor;
uniform float fogDensity;
uniform bool useShadows;

float ShadowCalculation(vec4 lightSpacePos)
{
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0) return 0.0;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - 0.005 > pcfDepth ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main()
{
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 0.97, 0.9);
    vec3 ambient = 0.25 * vec3(0.75, 0.88, 1.0);
    
    float shadow = useShadows ? ShadowCalculation(fragPosLightSpace) : 0.0;
    vec3 texColor = texture(colorTexture, texCoord).rgb;
    vec3 resultColor = (ambient + (1.0 - shadow) * diffuse) * texColor;
    
    float dist = length(cameraPos - fragPos);
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);
    
    FragColor = vec4(mix(fogColor, resultColor, fogFactor), 1.0);
}