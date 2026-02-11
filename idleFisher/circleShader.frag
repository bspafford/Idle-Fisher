#version 430 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;
flat in uint instanceIndex;

in vec2 TexCoord;

uniform float pixelSize;
uniform vec2 screenSize;
uniform float percent;

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

    float biggerSize = max(screenSize.x, screenSize.y);
    float radius = percent * biggerSize / 2.f;

    vec2 center = screenSize / 2.f;
    vec2 roundedCoords = floor(gl_FragCoord.xy / pixelSize) * pixelSize;
    
    FragColor = distance(roundedCoords, center) < radius ? vec4(0) : data.color;
}