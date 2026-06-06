#version 330 core

in vec2 screenPos;   // pozycja na ekranie (-1..1)
out vec4 FragColor;

uniform mat4  invViewProj;
uniform vec3  cameraPos;
uniform vec3  lightPos;      // pozycja slonca nad woda
uniform float time;
uniform mat4  viewProj;

void main()
{
    // --------------------------------------------------
    // 1. Tarcza sloneczna na ekranie (zostaje jak jest)
    // --------------------------------------------------
    vec4 lightClip = viewProj * vec4(lightPos, 1.0);
    vec2 lightScreen = lightClip.xy / lightClip.w;
    if (lightClip.w < 0.0)
        lightScreen = vec2(0.0, 2.0);

    vec2 toSun = screenPos - lightScreen;
    float sunDist = length(toSun);

    float sunGlow = exp(-sunDist * sunDist * 12.0);
    float sunHalo = exp(-sunDist * 2.5);
    float sunDisk = sunGlow * 0.9 + sunHalo * 0.35;

    // --------------------------------------------------
    // 2. Odtwarzamy kierunek patrzenia w przestrzeni swiata
    //    Dzieki temu promienie zmieniaja sie gdy obracamy kamere
    // --------------------------------------------------
    vec4 wNear = invViewProj * vec4(screenPos, -1.0, 1.0);
    vec4 wFar  = invViewProj * vec4(screenPos,  1.0, 1.0);
    wNear /= wNear.w;
    wFar  /= wFar.w;
    vec3 rayDir = normalize(wFar.xyz - wNear.xyz);

    // --------------------------------------------------
    // 3. Idziemy wzdluz promienia wzroku w przestrzeni swiata
    //    i zbieramy swiatlo z pionowych pasow
    // --------------------------------------------------
    float totalLight = 0.0;

    const int   NUM_STEPS = 24;    // wiecej krokow = dalej siegaja
    const float STEP_SIZE = 2.5;   // wiekszy krok
    const float START_DIST = 4.0;  // promienie zaczynaja sie od tej odleglosci

    for (int i = 0; i < NUM_STEPS; i++)
    {
        float t = START_DIST + float(i) * STEP_SIZE;
        vec3  p = cameraPos + rayDir * t;

        // Pomijamy tylko to co jest ponizej dna
        if (p.y < -10.0) continue;

        // Wzor promieni oparty na pozycji w swiecie (XZ)
        // Dzieki temu pattern zmienia sie gdy obracamy kamere
        float wx = p.x * 0.3;
        float wz = p.z * 0.3;

        float rays = 0.0;

        // Szerokie promienie
        rays += (sin(wx * 2.0  + time * 0.2 + wz * 0.5) * 0.5 + 0.5) * 1.0;
        rays += (sin(wz * 2.5  - time * 0.15 + wx * 0.3 + 1.7) * 0.5 + 0.5) * 0.7;

        // Srednie promienie
        rays += (sin(wx * 4.0 + wz * 1.5 + time * 0.1 + 3.1) * 0.5 + 0.5) * 0.45;
        rays += (sin(wz * 5.0 - wx * 1.2 - time * 0.12 + 5.3) * 0.5 + 0.5) * 0.3;

        // Drobne promienie
        rays += (sin(wx * 8.0 + wz * 3.0 + time * 0.08 + 2.0) * 0.5 + 0.5) * 0.2;

        rays = rays / 2.65;
        rays = pow(rays, 2.5);

        // Im blizej powierzchni, tym jasniej (bez gornego limitu)
        float depthFade = smoothstep(-10.0, 5.0, p.y);
        depthFade = pow(depthFade, 0.4);

        // Dalsze probki sa slabsze
        float distFade = exp(-(t - START_DIST) * 0.025);

        totalLight += rays * depthFade * distFade;
    }

    totalLight = totalLight / float(NUM_STEPS);
    totalLight = pow(totalLight, 0.6) * 1.8;
    totalLight = clamp(totalLight, 0.0, 1.0);

    // --------------------------------------------------
    // 4. Laczymy promienie + tarcze sloneczna
    // --------------------------------------------------
    float intensity = totalLight * 0.9 + sunDisk;
    intensity = clamp(intensity, 0.0, 1.0);

    // Kolor - cieplejszy blizej slonca, blekitny dalej
    vec3 sunColor  = vec3(0.95, 0.95, 0.85);
    vec3 rayColor  = vec3(0.4, 0.8, 0.95);
    float colorMix = smoothstep(0.0, 0.5, sunDist);
    vec3 finalColor = mix(sunColor, rayColor, colorMix);

    float alpha = intensity * 0.75;

    FragColor = vec4(finalColor * intensity, alpha);
}
