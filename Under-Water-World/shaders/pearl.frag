#version 330 core

in vec3 fragPos;
in vec3 normalWorld;
in vec2 texCoord;

out vec4 FragColor;

uniform samplerCube environmentMap;

uniform vec3 cameraPos;
uniform vec3 baseColor;

uniform float f0;
uniform float fresnelPower;
uniform float refractionRatio;
uniform float reflectionStrength;
uniform float refractionStrength;
uniform float time;

uniform vec3 fogColor;
uniform float fogDensity;

void main()
{
    vec3 normal = normalize(normalWorld);

    // Kierunek od fragmentu do kamery.
    vec3 viewDir = normalize(cameraPos - fragPos);

    // Kierunek promienia "od kamery do powierzchni" (reflect/retract).
    vec3 incident = normalize(fragPos - cameraPos);

    vec3 reflectedDir = reflect(incident, normal);
    vec3 refractedDir = refract(incident, normal, refractionRatio);

    vec3 reflectedColor = texture(environmentMap, reflectedDir).rgb;
    vec3 refractedColor = texture(environmentMap, refractedDir).rgb;

    float cosTheta = max(dot(viewDir, normal), 0.0);

    float fresnel = f0 + (1.0 - f0) * pow(1.0 - cosTheta, fresnelPower);
    fresnel = clamp(fresnel, 0.0, 1.0);

    //Lolor zmienia sie pod katem patrzenia.
    float viewAngle = 1.0 - cosTheta;
    float pearlWave = dot(normal, viewDir) * 8.0 + viewAngle * 10.0 + time * 0.25;

    vec3 pearlShift = 0.5 + 0.5 * cos(vec3(0.0, 2.0, 4.0) + pearlWave);
    vec3 pearlBase = baseColor * mix(vec3(0.92), pearlShift, 0.25 + fresnel * 0.35);

    vec3 environmentColor =
        reflectedColor * reflectionStrength * (0.35 + fresnel) +
        refractedColor * refractionStrength * (1.0 - fresnel);

    // Im wiekszy Fresnel, tym mocniej widac odbicie na krawedziach.
    vec3 finalColor = mix(
        pearlBase,
        environmentColor,
        0.25 + fresnel * 0.55
    );

    // Lekka mgła
    float dist = length(cameraPos - fragPos);
    float fogFactor = exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    finalColor = mix(fogColor, finalColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}