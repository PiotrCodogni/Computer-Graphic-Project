#version 330 core

// Fullscreen quad – pozycja wierzcholka
layout(location = 0) in vec3 position;

// Pozycja na ekranie przekazywana do fragment shadera
out vec2 screenPos;

void main()
{
    screenPos    = position.xy;
    gl_Position  = vec4(position.xy, 0.999, 1.0);
}
