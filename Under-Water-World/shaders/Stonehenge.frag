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

// PBR Constants
const float PI = 3.14159265359;

// Funkcje PBR
float dist_ggx(vec3 n, vec3 h, float rgh) {
    float a = rgh * rgh;
    float a2 = a * a;
    float n_dot_h = max(dot(n, h), 0.0);
    float n_dot_h2 = n_dot_h * n_dot_h;

    float num = a2;
    float denom = (n_dot_h2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float geom_schlick_ggx(float n_dot_v, float rgh) {
    float r = (rgh + 1.0);
    float k = (r * r) / 8.0;

    float num = n_dot_v;
    float denom = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geom_smith(vec3 n, vec3 v, vec3 l, float rgh) {
    float n_dot_v = max(dot(n, v), 0.0);
    float n_dot_l = max(dot(n, l), 0.0);

    float ggx2 = geom_schlick_ggx(n_dot_v, rgh);
    float ggx1 = geom_schlick_ggx(n_dot_l, rgh);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_t, vec3 f0) {
    return f0 + (1.0 - f0) * pow(clamp(1.0 - cos_t, 0.0, 1.0), 5.0);
}

void main() {
    vec3 alb = pow(texture(colorTexture, texCoord).rgb, vec3(2.2));
    vec3 n = normalize(fragNormal);
    vec3 v = normalize(cameraPos - fragPos);

    float met = 0.0;
    float rgh = 0.95;

    vec3 f0 = vec3(0.04);
    f0 = mix(f0, alb, met);

    vec3 l = normalize(lightPos - fragPos);
    vec3 h = normalize(v + l);

    float distL = length(lightPos - fragPos);
    float att = 1.0 / (distL * distL);
    vec3 rad = vec3(300000.0) * att;

    float ndf = dist_ggx(n, h, rgh);
    float g = geom_smith(n, v, l, rgh);
    vec3 f = fresnel_schlick(max(dot(h, v), 0.0), f0);

    vec3 num = ndf * g * f;
    float denom = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.0001;
    vec3 spec = (num / denom) * 0.15;

    vec3 ks = f;
    vec3 kd = vec3(1.0) - ks;
    kd *= 1.0 - met;

    float n_dot_l = max(dot(n, l), 0.0);

    float f_ao = clamp(0.5 + 0.5 * n.y, 0.0, 1.0);

    vec3 lo = (kd * alb / PI + spec) * rad * n_dot_l * f_ao;
    vec3 amb = vec3(0.05) * alb * f_ao;

    vec3 resCol = amb + lo;

    resCol = resCol / (resCol + vec3(1.0));
    resCol = pow(resCol, vec3(1.0 / 2.2));

    float dFog = length(cameraPos - fragPos);
    float fFac = clamp(exp(-dFog * fogDensity), 0.0, 1.0);

    FragColor = vec4(mix(fogColor, resCol, fFac), 1.0);
}
