#version 430 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
flat out uint instanceIndex;

uniform mat4 projection;
uniform vec2 playerPos;
uniform float pixelSize;

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
    instanceIndex = gl_InstanceID;
    InstanceData data = instances[instanceIndex];

    gl_Position = projection * vec4((aPos * data.size + data.position) * pixelSize - (playerPos * data.useWorldPos), 0.0, 1.0);

    // if source size is 0
    if (data.source.z == 0 || data.source.w == 0)
        TexCoord = aTexCoord; // (0, 0, 1, 1)
    else
        TexCoord = data.source.xy + aTexCoord * data.source.zw;
}
