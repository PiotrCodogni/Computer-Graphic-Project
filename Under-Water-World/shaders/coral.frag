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
uniform float metallic;
uniform float roughness;

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
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(cameraPos - fragPos);
    vec3 L = normalize(lightPos - fragPos);
    vec3 H = normalize(L + V); // Half-vector dla Blinn-Phong

    // 1. Diffuse (z tekstury)
    vec3 albedo = texture(colorTexture, texCoord).rgb;
    
    // 2. Specular (PBR-like approximation)
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, 1.0 / (roughness + 0.001)); // Roughness wp³ywa na rozmycie odb³ysku
    
    // Uproszczony Fresnel (fresnel = 1 dla k¹tów patrzenia)
    float F = pow(1.0 - max(dot(N, V), 0.0), 5.0); 
    vec3 specular = spec * vec3(0.5 + 0.5 * metallic) * (1.0 - F);

    // 3. Oœwietlenie
    float shadow = useShadows ? ShadowCalculation(fragPosLightSpace) : 0.0;
    vec3 diffuse = NdotL * vec3(1.0, 0.97, 0.9);
    vec3 ambient = 0.25 * vec3(0.75, 0.88, 1.0);
    
    vec3 resultColor = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;


    float dist = length(cameraPos - fragPos);
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);
    
    FragColor = vec4(mix(fogColor, resultColor, fogFactor), 1.0);
}