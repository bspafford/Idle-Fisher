#version 450 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 TexCoord;
flat in uint instanceIndex;
uniform int drawingToFBO;

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
        textureColor = texture(data.tex, TexCoord);
    FragColor = textureColor * data.color;

    if (drawingToFBO == 1) {
        //FragColor = data.color;
        //FragColor = vec4(TexCoord, 0, 1);
        //FragColor = vec4(data.size, 0, 1);
    }
}