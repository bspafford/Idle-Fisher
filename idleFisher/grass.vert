#version 430 core
#extension GL_ARB_bindless_texture : require

layout(location = 0) in vec2 aPos;  // Object vertex position
layout(location = 1) in vec2 aTexCoord;

uniform mat4 projection;
uniform vec2 playerPos;
uniform float pixelSize;
uniform vec2 screenSize;
uniform float time;

out vec2 texCoord;
out vec2 loc;
out flat int isAccent;

uint initRNG(uint id) {
	uint seed = id * 1973u;
	return seed | 1u; // avoid zero state
}

uint pcg(inout uint state) {
	uint old = state;
	state = old * 747796405u + 2891336453u;
	uint xorshifted = ((old >> ((old >> 28u) + 4u)) ^ old) * 277803737u;
	return (xorshifted >> 22u) ^ xorshifted;
}

float rand(inout uint state) {
	return float(pcg(state)) * (1.0 / 4294967296.0);
}

vec2 rand2(inout uint state) {
	return vec2(rand(state), rand(state));
}   

mat2 rotation2D(float a) {
	float s = sin(a);
	float c = cos(a);

	return mat2(
		c, -s,
		s,  c
	);
}

void main() {
	int id = gl_InstanceID;

	uint seed = initRNG(id);

	vec2 tempSize = vec2(22, 21);

	float randVal = rand(seed);
	tempSize.y *= mix(0.75, 1.25, randVal);
	if (randVal >= 0.99f)
		isAccent = 1;


	float radius = 150.f;
	vec2 randLoc = rand2(seed) * screenSize;
	float windRot = sin(randLoc.x + time / 1000.f) / 10.f;
	vec2 windPos = rotation2D(windRot) * aPos;
	vec2 newLoc = windPos * tempSize * pixelSize + randLoc - playerPos;
	float xDist = distance(randLoc.x - playerPos.x, screenSize.x / 2.f);
	float yDist = distance(randLoc.y - playerPos.y, screenSize.y / 2.f) * 2.f;
	if (xDist*xDist + yDist*yDist < radius*radius) {
		float newYSize = 0;
		if (randLoc.y - playerPos.y < screenSize.y / 2.f)
			newYSize = tempSize.y * mix(0.75, 1.f, yDist / radius); // if player below, it should stretch, otherwise getsmaller
		else
			newYSize = tempSize.y * mix(1.25f, 1.f, yDist / radius); // if player below, it should stretch, otherwise getsmaller

		tempSize = mix(vec2(tempSize.x, newYSize), tempSize, xDist / radius);

		vec2 rotPos = vec2(0, 0);
		if (randLoc.x - playerPos.x < screenSize.x / 2.f)
			rotPos = rotation2D(xDist / radius-1) * aPos;
		else
			rotPos = rotation2D(1-xDist / radius) * aPos;
		rotPos = mix(rotPos, windPos, yDist / radius);

		newLoc = rotPos * tempSize * pixelSize + randLoc - playerPos; // make this only effect x dir
	}

	gl_Position = projection * vec4(newLoc, 1 - randLoc.y / screenSize.y, 1.0);
	texCoord = aTexCoord;
	loc = randLoc;
}