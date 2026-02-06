#version 430 core
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec2 texCoord;
in vec2 loc;
in flat int isAccent;

uniform sampler2D grass;
uniform vec2 screenSize;

/*
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

//
// Description : GLSL 2D simplex noise function
//      Author : Ian McEwan, Ashima Arts
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License :
//  Copyright (C) 2011 Ashima Arts. All rights reserved.
//  Distributed under the MIT License. See LICENSE file.
//  https://github.com/ashima/webgl-noise
//
float snoise(vec2 v) {

	// Precompute values for skewed triangular grid
	const vec4 C = vec4(0.211324865405187,
						// (3.0-sqrt(3.0))/6.0
						0.366025403784439,
						// 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,
						// -1.0 + 2.0 * C.x
						0.024390243902439);
						// 1.0 / 41.0

	// First corner (x0)
	vec2 i  = floor(v + dot(v, C.yy));
	vec2 x0 = v - i + dot(i, C.xx);

	// Other two corners (x1, x2)
	vec2 i1 = vec2(0.0);
	i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
	vec2 x1 = x0.xy + C.xx - i1;
	vec2 x2 = x0.xy + C.zz;

	// Do some permutations to avoid
	// truncation effects in permutation
	i = mod289(i);
	vec3 p = permute(
			permute( i.y + vec3(0.0, i1.y, 1.0))
				+ i.x + vec3(0.0, i1.x, 1.0 ));

	vec3 m = max(0.5 - vec3(
						dot(x0,x0),
						dot(x1,x1),
						dot(x2,x2)
						), 0.0);

	m = m*m ;
	m = m*m ;

	// Gradients:
	//  41 pts uniformly over a line, mapped onto a diamond
	//  The ring size 17*17 = 289 is close to a multiple
	//      of 41 (41*7 = 287)

	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;

	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt(a0*a0 + h*h);
	m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

	// Compute final noise value at P
	vec3 g = vec3(0.0);
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
	return 130.0 * dot(m, g);
}

void main() {
	vec2 st = loc/screenSize;
	st.x *= screenSize.x/screenSize.y;

	// Scale the space in order to see the function
	st *= 2.0;

	float value = snoise(st) * 0.5 + 0.5;

	vec4 grassColor1 = vec4(107.0/255.0, 132.0/255.0, 45.0/255.0, 1.0);
	vec4 grassColor2 = vec4(120.0/255.0, 158.0/255.0, 36.0/255.0, 1.0);
	vec4 grassColor3 = vec4(150.0/255.0, 154.0/255.0, 38.0/255.0, 1.0);

	vec4 color;
	if (value <= 0.3333333)
		color = grassColor1;
	else if (value <= 0.66666666)
		color = grassColor2;
	else
		color = grassColor3;

	FragColor = texture(grass, texCoord) * color; // vec4(vec3(value), 1);// 
}
*/

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

	vec3 grassColor1 = vec3(107.0/255.0, 132.0/255.0, 45.0/255.0);
	vec3 grassColor2 = vec3(120.0/255.0, 158.0/255.0, 36.0/255.0);
	vec3 grassColor3 = vec3(150.0/255.0, 154.0/255.0, 38.0/255.0);
	vec3 color = pickPaletteColor(noise, grassColor1, grassColor2, grassColor3);
	vec3 reverseColor = pickPaletteColor(noise, grassColor2, grassColor3, grassColor1);

	uint seed = initRNG(uvec2(loc));

	if (texture(grass, texCoord).a < 0.5) discard; // allows transparency with depth testing

	if (isAccent == 1)
		FragColor = texture(grass, texCoord) * vec4(reverseColor, 1.0);
	else
		FragColor = texture(grass, texCoord) * vec4(color, 1.0);
}