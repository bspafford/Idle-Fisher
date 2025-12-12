#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec2 screenSize;
uniform vec4 color;
uniform float percent;

void main() {
    float biggerSize = max(screenSize.x, screenSize.y);
    float radius = percent * biggerSize / 2.f;

    vec2 center = screenSize / 2.f;
    vec2 roundedCoords = floor(gl_FragCoord.xy / 3) * 3;
    if (distance(roundedCoords, center) < radius)
        FragColor = vec4(0);
    else
        FragColor = color;
}