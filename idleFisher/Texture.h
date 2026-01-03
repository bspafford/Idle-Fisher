#pragma once

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include "math.h"

class Texture {
public:
	// For textures with images with render queuing
	Texture(const char* imgPath);
	// For textures assigned to things like FBOs with render queuing
	// { 0, 0 } will fill up the whole screen
	Texture(vector size);
	// Works for shaders that require multiple textures
	Texture(const char* imgPath, bool binding);

	~Texture();

	void LoadGPU();

	void Resize(vector size);

	// Assigns a texture unit to a texture
	void texUnit(Shader* shader, const char* uniform);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	GLuint64 GetHandle();
	GLuint64 GetID();


	static inline std::vector<std::unique_ptr<Texture>> textureCache;

	// binding stuff
	static void bindTextureToShader(Shader* shaderProgram, const char* path, const char* uniform);
	static void bindTextureToShader(std::vector<Shader*> shaderPrograms, const char* path, const char* uniform);
	static void deleteCache();
	// loops through each instances, resizing them when the screen size changes, if they fill the screen
	static void ResizeAllTextures();

private:
	static inline std::vector<Texture*> instances;

	GLuint ID = 0;
	GLuint unit = 0;
	GLuint64 handle = 0;

	// false means slot isn't used, true means slot is currently being used
	static inline std::vector<bool> usedSlots;
	static GLuint takeOpenSlot();
	static void releaseSlot(GLuint slot);

	// gpu load stuff
	int functionIdx = -1;
	const char* imgPath;
	unsigned char* bytes;
	int widthImg, heightImg, numColCh;
	vector size;
};