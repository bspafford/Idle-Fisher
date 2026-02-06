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

vec2 rotation2D(vec2 p, float a) {
	vec2 pivot = vec2(0.5, 0);
	p -= pivot;
	float s = sin(a);
    float c = cos(a);
    vec2 rotated = vec2(
        c * p.x - s * p.y,
        s * p.x + c * p.y
    );
	rotated += pivot;
	return rotated;
}

void main() {
	int id = gl_InstanceID;

	uint seed = initRNG(id);

	int grassNum = 2;
	vec2 grassSize = vec2(22, 21);

	vec2 randLoc = rand2(seed) * screenSize;

	float maxYScale = 0.25f;
	float maxRotX = 1.f;
	float radius = 150.f;

	vec2 grassLoc = randLoc - playerPos;
	vec2 charLoc = screenSize / 2.f;
    vec2 delta = grassLoc - charLoc;

    // Isometric scaling
    delta.y *= 2.f;

    float dist = length(delta);
    float fade = 1.0 - clamp(dist / radius, 0.0, 1.0); // how strong the effect should be at this distance

	// how far from center
    float ax = abs(delta.x);
    float ay = abs(delta.y);

    float verticalMask   = ay / (ax + ay + 1e-5); // up/down, [0,1] how vertical is this direction compared to horizontal, above and below = 1, left and right = 0
    float horizontalMask = 1.0 - verticalMask;    // left/right, inverse of vertical mask

    // Apply deformation
    vec2 pos = vec2(aPos);

    // X-axis rotation (stronger above/below)
    float rotX = maxRotX * verticalMask * fade; // max stretch * how up or down * how close to center
	rotX *= charLoc.x < grassLoc.x ? -1 : 1; // invert rotation is character is on left

	// wind
	float fps = 3.f;
	float roundedTime = floor((time + rand(seed)) * fps) / fps; // rand for a random offset so all grass isn't on the same update time, so it doesn't look laggy
	float windRot = sin(randLoc.x + roundedTime) / 10.f;

	pos = rotation2D(pos, windRot + rotX);

    // Y-axis stretch (stronger left/right)
    float yScale = maxYScale * horizontalMask * fade; // max scale * how left or right * how close to center
    pos.y *= charLoc.y < grassLoc.y ? (1 + yScale) : (1 - yScale);

	// accents
	if (rand(seed) >= 0.99f) {
		isAccent = 1;
		pos.y *= 1.25f;
		texCoord = vec2(aTexCoord.x * (1.f / grassNum) + (1.f / grassNum), aTexCoord.y);
	} else
		texCoord = vec2(aTexCoord.x * (1.f / grassNum), aTexCoord.y);

    // Final position
	gl_Position = projection * vec4(pos* pixelSize * grassSize + randLoc - playerPos, 1.f - randLoc.y / screenSize.y, 1.0);

	//texCoord = vec2(aTexCoord.x * (1.f / grassNum) + round(rand(seed)) * (1.f / grassNum), aTexCoord.y);
	loc = randLoc;
}