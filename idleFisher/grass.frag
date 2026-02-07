#version 430 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 texCoord;
in vec2 loc;
in flat int isAccent;
in flat float rot;
in flat vec2 temp;
in flat vec3 color;
in flat vec2 size;

uniform sampler2D grass;
uniform sampler2D tallGrass;
uniform vec2 screenSize;
uniform int isDepthPass;
uniform vec3 grassColor1;
uniform vec3 grassColor2;
uniform vec3 grassColor3;
uniform vec3 grassHighlight1;
uniform vec3 grassHighlight2;

vec3 pickPaletteColor(vec3 noise, vec3 c0, vec3 c1, vec3 c2) {
	vec3 mask = step(noise.gbr, noise.rgb) * step(noise.brg, noise.rgb);
	return c0 * mask.r + c1 * mask.g + c2 * mask.b;
}

uint initRNG(uvec2 loc) {
	uint seed = loc.x * 1973u
			  + loc.y * 9277u;
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

vec2 rotation2D(vec2 p, float a) {
	float s = sin(a);
	float c = cos(a);
	return vec2(
		c * p.x - s * p.y,
		s * p.x + c * p.y
	);
}

// Simple and fast 2D noise
float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// 2D Perlin-style noise
float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);

	// Smooth interpolation
	vec2 u = f*f*(3.0-2.0*f);

	// Mix 4 corners
	float a = hash(i + vec2(0.0, 0.0));
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

// Fractal / turbulence for more organic look
float fbm(vec2 p) {
	float f = 0.0;
	f += 0.5000 * noise(p);
	f += 0.2500 * noise(p*2.0);
	f += 0.1250 * noise(p*4.0);
	f += 0.0625 * noise(p*8.0);
	return f;
}

// Usage in fragment shader for 3-color blending
vec3 getGrassColor(vec2 uv, vec3 grassColor1, vec3 grassColor2, vec3 grassColor3) {
	float n = fbm(uv * 5.0);  // scale noise
	float n1 = fbm((uv + 100.0) * 5.0); // different input for variation

	vec3 color;
	if (n > 0.55) color = grassColor1;
	else if (n1 > 0.55) color = grassColor2;
	else color = grassColor3;

	return color;
}

// pre generate perlin noise / locations for grass
	// send in like a ssbo or vbo with like vec3 loc, int colorIndex (0-5)

void main() {
	// rotate in frag shader for pixelated look
	// rotates in the vert shader for geometry
	// rotates back, pixelates the coords, then rotates back
	vec2 pivot = vec2(0.5, 0.0);
	vec2 p = texCoord;
	p -= pivot;
	p = rotation2D(p, rot);
	p += pivot;
	p = floor(p * size) / size;
	p -= pivot;
	p = rotation2D(p, -rot);
	p += pivot;

	vec4 tex = isAccent == 1 ? texture(tallGrass, p) : texture(grass, p);
	bool outOfBounds = any(lessThan(p, vec2(0.0))) || any(greaterThan(p, vec2(1.0)));
	if (tex.a < 0.5 || outOfBounds) discard; // allows transparency with depth testing

	if (isDepthPass == 1)
		return;

	uint seed = initRNG(uvec2(loc));
	vec3 grassColor = isAccent == 1 ? mix(grassHighlight1, grassHighlight2, round(rand(seed))) : color;

	vec4 final = tex * vec4(grassColor, 1.f);
	FragColor = final;
}