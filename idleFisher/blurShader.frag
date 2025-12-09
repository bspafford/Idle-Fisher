#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform int radius;
uniform vec2 screenSize;

void main() {
    if (radius == 0) {
        FragColor = texture(screenTexture, TexCoord);
        return;
    }

    vec2 texelSize = 1 / screenSize;

    vec3 result = vec3(0.0);
    int total = 0;

    // Simple 3×3 box blur
    for (int x = -radius; x <= radius; x += 3) {
        for (int y = -radius; y <= radius; y += 3) {
            vec2 offset = vec2(x, y) * texelSize;
            vec2 coords = TexCoord + offset;
        
            // Adjusting the offset to compensate for the shift
            coords += 0.333f * vec2(radius, radius) * texelSize;
        
            if (coords.x >= 0.0 && coords.x <= 1.0 && coords.y >= 0.0 && coords.y <= 1.0) {
                result += texture(screenTexture, coords).rgb;
                total++;
            }
        }
    }

    FragColor = vec4(result / total, 1.0);
}
