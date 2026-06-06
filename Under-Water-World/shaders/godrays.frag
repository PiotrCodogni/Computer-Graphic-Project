#version 330 core

in vec2 screenPos;   // pozycja na ekranie (-1..1)
out vec4 FragColor;

// Uniformy kamery i sceny
uniform mat4  invViewProj;   // odwrocona macierz widoku-projekcji
uniform vec3  cameraPos;
uniform vec3  lightPos;      // pozycja slonca nad woda
uniform float time;

// -------------------------------------------------------
// Oblicza jasnosc promieni slonecznych w danym punkcie swiata.
// Kazdy promien to pionowa kolumna swiatla ktora sie porusza
// symulujac falowanie powierzchni wody.
// -------------------------------------------------------
float shaftIntensityAt(vec3 p, float t)
{
    const int N_SHAFTS = 7;  // liczba promieni
    float total = 0.0;

    for (int j = 0; j < N_SHAFTS; j++)
    {
        float fj = float(j);

        // Rozklad promieni pod roznym katem
        float baseAngle = fj * 0.8976;

        // Ruch promienia w X i Z - symuluje falowanie wody
        float swayX = sin(t * 0.22 + fj * 1.57) * 4.5
                    + cos(t * 0.13 + fj * 0.89) * 2.0;
        float swayZ = cos(t * 0.18 + fj * 1.23) * 4.5
                    + sin(t * 0.16 + fj * 1.05) * 2.0;

        // Odleglosc promienia od srodka - im dalszy, tym bardziej na zewnatrz
        float fanRadius = 2.0 + fj * 2.2;

        vec2 shaftCenter = vec2(
            lightPos.x + cos(baseAngle) * fanRadius + swayX,
            lightPos.z + sin(baseAngle) * fanRadius + swayZ
        );

        float d     = length(p.xz - shaftCenter);
        float width = 1.8 + fj * 0.35; // zewnetrzne promienie sa szersze

        // Gasnacia jasnosci z odlegloscia od srodka promienia
        float g = exp(-d * d / (width * width));

        // Migotanie - kazdy promien pulsuje niezaleznie
        float flicker = 0.75 + 0.25 * sin(t * 1.4 + fj * 2.3);

        total += g * flicker;
    }

    return total / float(N_SHAFTS);
}

void main()
{
    // --------------------------------------------------
    // 1. Odtwarzamy kierunek patrzenia dla tego piksela
    // --------------------------------------------------
    vec4 wNear = invViewProj * vec4(screenPos, -1.0, 1.0);
    vec4 wFar  = invViewProj * vec4(screenPos,  1.0, 1.0);
    wNear /= wNear.w;
    wFar  /= wFar.w;

    vec3 rayDir = normalize(wFar.xyz - wNear.xyz);

    // Promienie widac tylko gdy patrzymy do gory, nie w dol
    float upwardMask = clamp(rayDir.y * 2.5 + 0.3, 0.0, 1.0);
    if (upwardMask <= 0.001)
    {
        FragColor = vec4(0.0);
        return;
    }

    // --------------------------------------------------
    // 2. Idziemy wzdluz promienia wzroku i zbieramy swiatlo
    // --------------------------------------------------
    float totalLight = 0.0;

    const int   NUM_STEPS = 14;   // ile probek bierzemy
    const float STEP_SIZE = 2.2;  // odleglosc miedzy probkami

    for (int i = 0; i < NUM_STEPS; i++)
    {
        float t = (float(i) + 0.5) * STEP_SIZE;
        vec3  p = cameraPos + rayDir * t;

        // Probkujemy tylko pod woda (miedzy powierzchnia a dnem)
        if (p.y > 2.0 || p.y < -8.5) continue;

        // Im blizej powierzchni, tym jasniej
        float depthFade = smoothstep(-8.5, 2.0, p.y);

        totalLight += shaftIntensityAt(p, time) * depthFade;
    }

    // Normalizacja i wygladzenie
    totalLight = totalLight / float(NUM_STEPS);
    totalLight = pow(totalLight, 0.65) * 0.9;
    totalLight = clamp(totalLight, 0.0, 1.0);

    // Ukryj promienie gdy patrzymy w dol
    totalLight *= upwardMask;

    // --------------------------------------------------
    // 3. Kolor wyjsciowy - jasny blekitny jak swiatlo w wodzie
    // --------------------------------------------------
    vec3 rayColor = vec3(0.38, 0.82, 1.0);
    float alpha   = totalLight * 0.72;

    FragColor = vec4(rayColor * totalLight, alpha);
}
