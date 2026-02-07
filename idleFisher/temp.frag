#version 450 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D depthTex;

void main() {
    //float depth = texture(depthTex, uv).r; // sample depth
    //FragColor = vec4(vec3(depth), 1.0);    // grayscale

    float depth = texture(depthTex, uv).r;
    //float nearPlane = -1.0;
    //float farPlane = 5000.0;
    //float linearDepth = (depth - nearPlane) / (farPlane - nearPlane);
    //depth = (depth - 0.0) / (1.0 - 0.1); // map near/far range
FragColor = vec4(vec3(depth), 1.0);
}
