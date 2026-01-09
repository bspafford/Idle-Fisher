#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
    vec3 crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
    gl_Position = lightSpaceMatrix * vec4(crntPos, 1.0);
    //gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}