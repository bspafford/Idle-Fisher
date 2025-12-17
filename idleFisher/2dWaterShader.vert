#version 430 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec2 aPos;  // Object vertex position
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec2 playerPos;
uniform float pixelSize;

out vec4 clipSpace;
out vec2 textureCoords;

struct InstanceData {
    vec2 position;
	int useWorldPos;
	int pad1;
    
    sampler2D tex;
    vec2 size;

    vec4 source;

    vec4 color;
};

layout(std430, binding = 0) buffer InstanceBuffer{ InstanceData instances[]; };

void main() {
	InstanceData data = instances[gl_InstanceID];
	gl_Position = projection * vec4(aPos.xy * data.size * pixelSize - playerPos, 0.0, 1.0);
	textureCoords = aTexCoord;
}