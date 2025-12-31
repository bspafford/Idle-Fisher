#version 430 core
#extension GL_ARB_bindless_texture : require

in vec2 textureCoords;
in vec4 clipSpace;

out vec4 FragColor;

uniform sampler2D dudvMap;
uniform sampler2D underwaterTexture;
uniform sampler2D causticTexture;
uniform sampler2D waterDepthTexture;
uniform sampler2D reflectionTexture;
uniform sampler2D underwaterObjectTexture;
uniform sampler2D underwaterObjectDepthMap;
uniform vec3 shallowWaterColor;
uniform vec3 deepWaterColor;
uniform float causticSize;
uniform vec2 waterImgSize;

uniform float moveFactor;
uniform float pixelSize;

const float waveStrength = 0.005f;

void main() {
	vec2 blockSize = vec2(1.0) / waterImgSize;
	vec2 pixelCoords = (floor(textureCoords / blockSize) + 0.5) * blockSize;
	pixelCoords = vec2(pixelCoords.x, 1.f - pixelCoords.y);

	vec2 reflectTexCoords = pixelCoords.xy;
	vec2 refractTexCoords = pixelCoords.xy;

	vec2 distortion1 = (texture(dudvMap, vec2((pixelCoords.x + moveFactor), pixelCoords.y )).rg * 2.0f - 1.0f) * waveStrength;
	vec2 totalDistortion = distortion1;

	reflectTexCoords += totalDistortion;
	reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
	refractTexCoords -= totalDistortion * .5f;
	refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

	vec4 underwaterColor = texture(underwaterTexture, refractTexCoords);
	if (underwaterColor.a == 0)
		underwaterColor = vec4(0.f);

	vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);

	// set default size
	vec4 caustics = texture(causticTexture, reflectTexCoords.xy * causticSize);

	float waterDepth = texture(waterDepthTexture, pixelCoords.xy).r + .1f; //  + .1f // give a lil offset
	vec4 waterColor = vec4(mix(deepWaterColor, shallowWaterColor, waterDepth), 1.f);

	float waterBrightness = (dot(waterColor.xyz, vec3(0.2126, 0.7152, 0.0722)) * waterColor.w); // r * .21 + b * .71 + ...
	vec4 causticsColor = vec4(clamp(caustics.r * (waterBrightness - .425f) * .35f, 0.f, 1.f));

	vec4 underwaterObjectColor = texture(underwaterObjectTexture, refractTexCoords);
	if (underwaterColor.a == 0 && underwaterObjectColor.a != 0 && underwaterObjectColor.rgb != vec3(0))
		underwaterColor = underwaterObjectColor;

	vec4 refractedColor = underwaterColor;
	if (refractedColor.a != 0)
		refractedColor = mix(waterColor, underwaterColor, waterBrightness - .25f) + causticsColor;

	vec4 result;
	if (reflectColor.a != 0 && refractedColor != 0)
		result = mix(refractedColor, reflectColor, .3f);
	else if (reflectColor.a != 0)
		result = mix(waterColor, reflectColor, .3f);
	else
		result = refractedColor;

	if (reflectColor.a == 0 && refractedColor.a == 0) {
		FragColor = waterColor + causticsColor;
	} else
		FragColor = result;

	// make the objects get less transparent as they get deeper
	vec4 testColor = texture(underwaterObjectDepthMap, refractTexCoords);
	if (testColor.a != 0)
		FragColor = mix(FragColor, mix(FragColor, underwaterObjectColor, 0.3), testColor.r - 0.5);
}