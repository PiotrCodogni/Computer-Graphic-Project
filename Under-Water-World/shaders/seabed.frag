#version 330 core

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform sampler2D normalMap;
uniform int isRock;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float time;   // czas sceny do animacji odblaskow
uniform vec3 fogColor;
uniform float fogDensity;

// -------------------------------------------------------
// Funkcje losowe - zamieniaja pozycje komorki na losowa wartosc
// -------------------------------------------------------
vec2 hash2(vec2 p)
{
    p = vec2(dot(p, vec2(127.1, 311.7)),
             dot(p, vec2(269.5, 183.3)));
    return fract(sin(p) * 43758.5453);
}

// Losowa wartosc dla dodatkowej roznorodnosci miedzy komorkami
float hash1(vec2 p)
{
    return fract(sin(dot(p, vec2(419.2, 371.9))) * 83947.3);
}

// -------------------------------------------------------
// Animowane odblaski słońca Na dnie morskim
//
// Prawdziwe odblaski tworza jasna siatke linii na granicach
// miedzy komorkami (kazda komorka = jedna soczewka na powierzchni wody).
// Jasne sa krawedzie komorek - tam swiatlo sie skupia.
//
// Kazdy punkt rusza sie niezaleznie wlasna sciezka,
// z losowa predkoscia i faza - dzieki temu kazdy odblask wyglada inaczej.
// -------------------------------------------------------
float voronoiCaustics(vec2 pos, float t)
{
    // Dwie warstwy o roznych skalach - daje bogatszy wzor
    float c = 0.0;

    for (int oct = 0; oct < 2; oct++)
    {
        float scale  = (oct == 0) ? 0.38 : 0.72;
        float weight = (oct == 0) ? 0.65 : 0.35;

        vec2 p = pos * scale;
        vec2 i = floor(p);
        vec2 f = fract(p);

        float d1 = 1e9, d2 = 1e9;

        // Sprawdzamy 9 sasiadujacych komorek (3x3)
        for (int y = -1; y <= 1; y++)
        {
            for (int x = -1; x <= 1; x++)
            {
                vec2 cell   = vec2(float(x), float(y));
                vec2 cellId = i + cell;

                // Losowa pozycja punktu w komorce (0..1)
                vec2 base = hash2(cellId);

                // Losowa predkosc i faza ruchu dla kazdej komorki
                float speed = 0.6 + hash1(cellId + 3.7) * 0.8;
                float phase = hash1(cellId + 13.1)      * 6.2831;

                // Ruch punktu - kazdy porusza sie po swojej sciezce
                vec2 pt = base
                    + 0.28 * vec2(sin(t * speed * 0.55 + phase),
                                  cos(t * speed * 0.43 + phase + 1.1))
                    + 0.12 * vec2(cos(t * speed * 1.1  + phase * 1.3),
                                  sin(t * speed * 0.9  + phase * 0.7));

                vec2  diff = cell + pt - f;
                float d    = dot(diff, diff); // odleglosc do kwadratu (szybsze)

                if (d < d1) { d2 = d1; d1 = d; }
                else if (d < d2) { d2 = d; }
            }
        }

        d1 = sqrt(d1);
        d2 = sqrt(d2);

        // Krawedzie komorek sa jasne - im blizej krawedzi, tym jasniej.
        // Odwracamy zeby krawedzie swiecily na bialo.
        float border = d2 - d1;
        border = clamp(border * 2.5, 0.0, 1.0);      // wygladzenie
        float caustic = 1.0 - border;                 // odwrocenie - jasne krawedzie
        caustic = pow(caustic, 4.5);                  // wyostrzenie

        c += caustic * weight;
    }

    return clamp(c, 0.0, 1.0);
}

void main()
{
   
    vec3 norm;
    if (isRock == 1)
    {
        vec3 normalFromMap = texture(normalMap, texCoord).rgb * 2.0 - 1.0;
        // Wzmocnienie wypuklosci normalmapy (zwiekszenie wektorow x,y)
        normalFromMap.xy *= 3.0; 
        norm = normalize(TBN * normalFromMap);
    }
    else
    {
        norm = normalize(fragNormal);
    }

    // --- Swiatlo otoczenia ---
    // Zeby normalmapa (wypuklosci) byla widoczna takze w cieniu,
    // ambient nie moze byc zupelnie staly. Uzalezniamy go delikatnie 
    // od normalnej (kierunek do kamery + swiatlo z gory).
    vec3  viewDir = normalize(cameraPos - fragPos);
    float viewFactor = max(dot(norm, viewDir), 0.0);
    float hemiFactor = 0.5 + 0.5 * norm.y;
    
    float ambientStrength = 0.45; 
    vec3  ambient = ambientStrength * vec3(0.75, 0.88, 1.0) * (0.2 + 0.6 * viewFactor + 0.2 * hemiFactor);

    // --- Swiatlo kierunkowe (slonce z gory) ---
    vec3  lightDir = normalize(lightPos - fragPos);
    float diff     = max(dot(norm, lightDir), 0.0);
    vec3  diffuse  = diff * vec3(1.0, 0.97, 0.9);

    //  Kolor materialu 
    vec3 texColor;
    if (isRock == 1)
    {
        texColor = vec3(0.55, 0.52, 0.50);
    }
    else
    {
        texColor = texture(colorTexture, texCoord).rgb;
    }
    vec3 resultColor = (ambient + diffuse) * texColor;

    // --- Odblaski na dnie ---
    float causticsVal   = voronoiCaustics(fragPos.xz, time);

    // Odblaski gasna z odlegloscia - w prawdziwej wodzie
    // widac je tylko na bliskich powierzchniach
    float causticDist   = length(cameraPos - fragPos);
    float causticFade   = exp(-causticDist * 0.07);  // szybki zanik
    causticFade = clamp(causticFade, 0.0, 1.0);

    // Jasny blekitno-bialy kolor odblaskow
    vec3  causticsColor = vec3(0.6, 0.95, 1.0) * causticsVal * 0.75 * causticFade;
    resultColor += causticsColor;

    // --- Mgla podwodna ---
    float dist       = length(cameraPos - fragPos);
    float fogFactor  = exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, resultColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
