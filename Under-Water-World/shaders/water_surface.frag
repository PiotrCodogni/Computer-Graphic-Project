#version 330 core

in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D normalMap;
uniform float     time;
uniform vec3      cameraPos;
uniform vec3      lightPos;

void main()
{
    // Dwie animowane warstwy fal (przesuniecia w roznych kierunkach)
    vec2 uv1 = texCoord * 1.5 + vec2( time * 0.015,  time * 0.010);
    vec2 uv2 = texCoord * 0.8 + vec2(-time * 0.012,  time * 0.018);

    vec3 n1 = texture(normalMap, uv1).rgb * 2.0 - 1.0;
    vec3 n2 = texture(normalMap, uv2).rgb * 2.0 - 1.0;
    vec3 blended = normalize(vec3((n1.xz + n2.xz) * 2.0, 1.0));
    vec3 norm = normalize(TBN * blended);

    
  
    
    // Kierunek patrzenia kamery (od dolu do gory)
    vec3 viewDir = normalize(cameraPos - fragPos);

    // Ile swiatla przebija sie przez fale w kierunku kamery?
    // Gdy normalna fali jest skierowana prosto ku kamerze (plaska fala) = max swiatlo
    // Gdy normalna jest odchylona = total internal reflection = ciemno
    float caustic = max(dot(-norm, viewDir), 0.0);
    // Ostre skupienie: chcemy wyrazne jasne plamy, nie rozmycie
    caustic = pow(caustic, 4.0);
    
    // Dodatkowa warstwa: ile slonca przebija pionowo (Y+) - daje ogolna poswiate
    float vertLight = max(dot(-norm, vec3(0.0, 1.0, 0.0)), 0.0);
    vertLight = pow(vertLight, 2.5);

    // Kolory:
 
    vec3 darkColor    = vec3(0.01, 0.04, 0.18);   // ciemny granat - tlo
    vec3 causticColor = vec3(0.85, 0.95, 1.00);   // biale-cyjankowe kaustyki
    vec3 midBlue      = vec3(0.10, 0.40, 0.80);   // sredni blek - poswiate

    vec3 surfaceColor = darkColor
                      + midBlue   * vertLight  * 0.5
                      + causticColor * caustic  * 1.5;

    float alpha = caustic * 0.80 + vertLight * 0.20;
    alpha = clamp(alpha, 0.0, 0.90);

    // Zanikanie tekstury w oddali (Radial Fade)
    float dist = distance(cameraPos, fragPos);
    // Zaczyna zanikac od odleglosci 600, a przy 2500 zanika calkowicie
    float fadeFactor = 1.0 - smoothstep(600.0, 2500.0, dist);
    
    alpha *= fadeFactor;

    FragColor = vec4(surfaceColor, alpha);
}
