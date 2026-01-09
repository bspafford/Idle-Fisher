#version 330 core
layout(location = 0) in vec3 aPos;  // Object vertex position
layout(location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat3 normalMatrix;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform mat4 camMatrix;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

out vec4 FragPosLightSpace;  // Pass the light space position to the fragment shader

out vec3 vNormal; // test normals

void main() {
	vec3 crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
    // Apply the transformation to world space, then light space
    vec4 worldPosition = model * vec4(aPos, 1.0);
    FragPosLightSpace = lightSpaceMatrix * vec4(crntPos, 1.0f); // Transform to light space

    gl_Position = camMatrix * vec4(crntPos, 1.0);//projection * view * worldPosition; // Normal MVP transformation

    vNormal = normalize(normalMatrix * inNormal);
}