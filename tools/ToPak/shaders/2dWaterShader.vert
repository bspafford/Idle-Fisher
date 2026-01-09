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
	InstanceData data = instances[gl_InstanceID];
	gl_Position = projection * vec4(aPos * data.size * pixelSize - playerPos * data.useWorldPos, 0.0, 1.0);
	textureCoords = aTexCoord;
}