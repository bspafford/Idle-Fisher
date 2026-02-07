#pragma once

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"
#include "math.h"

class textureStruct;

class Texture {
public:
	// For textures assigned to things like FBOs with render queuing
	// { 0, 0 } will fill up the whole screen
	Texture(vector size);
	// Works for shaders that require multiple textures
	Texture(const std::string& imgPath);

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

	static GLuint takeOpenSlot();
	static void releaseSlot(GLuint slot);
private:
	// false means slot isn't used, true means slot is currently being used
	static inline std::vector<bool> usedSlots;

	GLuint id = 0;
	GLuint unit = 0;
	GLuint64 handle = 0;

	// gpu load stuff
	int functionIdx = -1;
	textureStruct* texData;
	vector size;
};