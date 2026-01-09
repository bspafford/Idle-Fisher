#version 330 core
out vec4 FragColor;

in vec4 FragPosLightSpace;
in vec2 texCoord;
in vec3 vNormal;

uniform sampler2D shadowMap;
uniform vec3 color;

uniform int shadowOnly;

void main() {
	float bias = 0.002f;//.0005f;
    
	// Normalize the shadow coordinates
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;  // Perspective divide
    projCoords = projCoords * 0.5 + 0.5; // Map to [0, 1] range

    // Sample the shadow map
    float shadowDepth = texture(shadowMap, projCoords.xy).r;

    // Check if the fragment is in shadow (compare depths)
    float shadow = (projCoords.z - bias) > shadowDepth ? 0.5 : 1.0;

    // Final color (object color modulated by shadow factor)
	vec3 objectColor = vec3(1.f);
	vec3 lightColor = vec3(1.f);

	if (shadow <= .5) //  
		FragColor = vec4(0, 0, 0, .5f);
	else if (shadowOnly != 1)
		FragColor = vec4(vec3(0.38823529411, 0.69803921568, 0.81176470588) * shadow, 1.f); // // 68.f / 255.f, 148.f / 255.f, 176.f / 255.f
		//FragColor = vec4(vec3(0, 1, 1) * shadow, 1.f); // cyan

		//FragColor = vec4(1);
}