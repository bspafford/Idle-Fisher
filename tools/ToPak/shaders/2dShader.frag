#version 450 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 TexCoord;
flat in uint instanceIndex;

struct InstanceData {
    vec4 color;
    vec2 position;
	int useWorldPos;
	int hasTexture;
    
    sampler2D tex;
    vec2 size;

    vec4 source;
};

layout(std430, binding = 0) buffer InstanceBuffer{ InstanceData instances[]; };

void main() {
    InstanceData data = instances[instanceIndex];

    vec4 textureColor = vec4(1);
    if (data.hasTexture != 0)
        textureColor = texture(data.tex, vec2(TexCoord.x, 1 - TexCoord.y));
    FragColor = textureColor * data.color;
}