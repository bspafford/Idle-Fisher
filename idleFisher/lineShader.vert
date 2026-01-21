#version 330 core

layout(location = 0) in vec2 aPos;  // Input vertex position (2D)

uniform mat4 projection;  // Projection matrix for transformations
uniform vec2 playerPos;

void main()
{
    float pixelSize = 3.f;
    gl_Position = projection * vec4(aPos * pixelSize - playerPos, 0.0, 1.0);
}