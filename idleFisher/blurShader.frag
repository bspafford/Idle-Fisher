#version 430 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 TexCoord;
flat in uint instanceIndex;

uniform int radius;
uniform vec2 screenSize;

struct InstanceData {
    vec4 color;
    
    vec2 position;
	int useWorldPos;
	int hasTexture;

    int useDepth;
    int pad[3];
    
    sampler2D tex;
    vec2 size;

    vec4 source;
};

layout(std430, binding = 0) buffer InstanceBuffer{ InstanceData instances[]; };

void main() {
    InstanceData data = instances[instanceIndex];

    vec2 texCoord = vec2(TexCoord.x, 1.f - TexCoord.y);

    if (radius == 0) {
        FragColor = texture(data.tex, texCoord);
        return;
    }

    vec2 texelSize = 1 / screenSize;

    vec3 result = vec3(0.0);
    int total = 0;

    // Simple 3×3 box blur
    for (int x = -radius; x <= radius; x += 3) {
        for (int y = -radius; y <= radius; y += 3) {
            vec2 offset = vec2(x, y) * texelSize;
            vec2 coords = texCoord + offset;
        
            // Adjusting the offset to compensate for the shift
            coords += 0.333f * vec2(radius, radius) * texelSize;
        
            if (coords.x >= 0.0 && coords.x <= 1.0 && coords.y >= 0.0 && coords.y <= 1.0) {
                result += texture(data.tex, coords).rgb;
                total++;
            }
        }
    }

    FragColor = vec4(result / total, 1.0);
}
