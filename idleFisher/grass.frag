#version 430 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 texCoord;
in vec2 loc;
in flat int isAccent;

uniform sampler2D grass;
uniform float pixelSize;
uniform vec2 screenSize;

vec2 fade(vec2 t) {
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

vec2 grad2(float hash) {
	float a = hash * 6.2831853;
	return vec2(cos(a), sin(a));
}

float hash(float n) {
	return fract(sin(n) * 43758.5453123);
}

float perlin(vec2 p, float seed) {
	vec2 i = floor(p);
	vec2 f = fract(p);

	vec2 u = fade(f);

	float h00 = hash(dot(i + vec2(0.0, 0.0), vec2(127.1, 311.7)) + seed);
	float h10 = hash(dot(i + vec2(1.0, 0.0), vec2(127.1, 311.7)) + seed);
	float h01 = hash(dot(i + vec2(0.0, 1.0), vec2(127.1, 311.7)) + seed);
	float h11 = hash(dot(i + vec2(1.0, 1.0), vec2(127.1, 311.7)) + seed);

	float n00 = dot(grad2(h00), f - vec2(0.0, 0.0));
	float n10 = dot(grad2(h10), f - vec2(1.0, 0.0));
	float n01 = dot(grad2(h01), f - vec2(0.0, 1.0));
	float n11 = dot(grad2(h11), f - vec2(1.0, 1.0));

	float nx0 = mix(n00, n10, u.x);
	float nx1 = mix(n01, n11, u.x);
	float nxy = mix(nx0, nx1, u.y);

	return nxy;
}

vec3 rgbPerlin(vec2 uv) {
	return vec3(
		perlin(uv, 10.1),
		perlin(uv, 20.2),
		perlin(uv, 30.3)
	) * 0.5 + 0.5;
}

vec3 rgbPerlinColored(vec2 uv, float colorPower) {
	vec3 n = vec3(0.0);
	float amp = 1.0;
	float freq = 1.0;
	float norm = 0.0;

	for (int i = 0; i < 5; i++) {
		n += rgbPerlin(uv * freq) * amp;
		norm += amp;

		freq *= 2.0;
		amp *= pow(0.5, colorPower);
	}

	return n / norm;
}

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

void main() {
	vec3 noise = rgbPerlinColored(vec2(loc.x, loc.y*2.f)/300.0, 1.2);

	vec3 grassColor1 = vec3(68.0/255.0, 113.0/255.0, 25.0/255.0);
	vec3 grassColor2 = vec3(83.0/255.0, 120.0/255.0, 23.0/255.0);
	vec3 grassColor3 = vec3(56.0/255.0, 107.0/255.0, 4.0/255.0);

	vec3 grassColor4 = vec3(57.0/255.0, 99.0/255.0, 5.0/255.0);
	vec3 grassColor5 = vec3(96.0/255.0, 136.0/255.0, 9.0/255.0);

	vec3 color = pickPaletteColor(noise, grassColor1, grassColor2, grassColor3);

	uint seed = initRNG(uvec2(loc));

	if (texture(grass, texCoord).a < 0.5) discard; // allows transparency with depth testing

	vec3 grassColor = isAccent == 1 ? mix(grassColor4, grassColor5, round(rand(seed))) : color;
	FragColor = texture(grass, texCoord) * vec4(grassColor, 1.f);
}