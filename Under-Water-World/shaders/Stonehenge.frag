#version 330 core
in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float time;
uniform vec3 fogColor;
uniform float fogDensity;

// Wklejamy tutaj funkcje pomocnicze z seabed.frag
vec2 hash2(vec2 p) {
    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453);
}

float hash1(vec2 p) {
    return fract(sin(dot(p, vec2(419.2, 371.9))) * 83947.3);
}

float voronoiCaustics(vec2 pos, float t) {
    float c = 0.0;
    for (int oct = 0; oct < 2; oct++) {
        float scale = (oct == 0) ? 0.38 : 0.72;
        float weight = (oct == 0) ? 0.65 : 0.35;
        vec2 p = pos * scale;
        vec2 i = floor(p); vec2 f = fract(p);
        float d1 = 1e9, d2 = 1e9;
        for (int y = -1; y <= 1; y++) {
            for (int x = -1; x <= 1; x++) {
                vec2 cell = vec2(float(x), float(y));
                vec2 cellId = i + cell;
                vec2 base = hash2(cellId);
                float speed = 0.6 + hash1(cellId + 3.7) * 0.8;
                float phase = hash1(cellId + 13.1) * 6.2831;
                vec2 pt = base + 0.28 * vec2(sin(t * speed * 0.55 + phase), cos(t * speed * 0.43 + phase + 1.1))
                               + 0.12 * vec2(cos(t * speed * 1.1 + phase * 1.3), sin(t * speed * 0.9 + phase * 0.7));
                vec2 diff = cell + pt - f;
                float d = dot(diff, diff);
                if (d < d1) { d2 = d1; d1 = d; }
                else if (d < d2) { d2 = d; }
            }
        }
        d1 = sqrt(d1); d2 = sqrt(d2);
        float border = clamp((d2 - d1) * 2.5, 0.0, 1.0);
        float caustic = pow(1.0 - border, 4.5);
        c += caustic * weight;
    }
    return clamp(c, 0.0, 1.0);
}

void main() {
    // Oœwietlenie
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 0.97, 0.9);
    vec3 ambient = 0.25 * vec3(0.75, 0.88, 1.0);
    
    vec3 texColor = texture(colorTexture, texCoord).rgb;
    vec3 resultColor = (ambient + diffuse) * texColor;
    
    // Caustics
    float causticsVal = voronoiCaustics(fragPos.xz, time);
    float causticDist = length(cameraPos - fragPos);
    float causticFade = clamp(exp(-causticDist * 0.07), 0.0, 1.0);
    resultColor += vec3(0.6, 0.95, 1.0) * causticsVal * 0.75 * causticFade;
    
    // Mg³a
    float dist = length(cameraPos - fragPos);
    float fogFactor = clamp(exp(-dist * fogDensity), 0.0, 1.0);
    
    FragColor = vec4(mix(fogColor, resultColor, fogFactor), 1.0);
}