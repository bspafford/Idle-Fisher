#pragma once

#include "math.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <memory>
#include <stack>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

class Shader;

struct InstanceData {
	glm::vec4 color;
	glm::vec2 position;
	int useWorldPos;
	int hasTexture;

	GLuint64 tex;
	glm::vec2 size;

	glm::vec4 source;

	// tex == 0 means object doesn't have texture
	InstanceData(glm::vec2 _position, glm::vec2 _size, glm::vec4 _source, int _useWorldPos, glm::vec4 _color, uint64_t _tex)
		: position(_position), size(_size), source(_source), useWorldPos(_useWorldPos), color(_color), tex(_tex), hasTexture(_tex != 0) {}
};

struct textureStruct {
	// if useAlpha == true then if mouse over a == 0 then it won't trigger
	GLuint64 handle;
	bool useAlpha = false;
	int w = 0;
	int h = 0;
	int nChannels;
	std::vector<uint8_t> alphaBits; // stores which pixels are transparent for mouse over checks

	textureStruct(const std::string& path);
	~textureStruct() {}

	bool GetAlphaAtPos(vector pos);
};

class textureManager {
public:
	textureManager();
	static void Deconstructor();
	static textureStruct* loadTexture(std::string path);
	static textureStruct* getTexture(std::string name);

	static void StartFrame();
	// Adds the Image into a draw queue to render at end of frame or when shader is swapped
	static void DrawImage(Shader* shader, const vector& position, const vector& size, const Rect& source, const bool& useWorldPos, const glm::vec4& color, const uint64_t& texHandle);
	// Instantly draws the image, used for things like FBOs
	static void DrawRect(Shader* shader, const vector& position, const vector& size, const bool& useWorldPos, const glm::vec4& color);
	static void EndFrame();
	// Uploads data to GPU when called
	// Used for things like FBO changes or with something like the blur box
	static void ForceGPUUpload();

	static GLuint GetSamplerID();

private:
	// checks if the shader has changed
	static void CheckShaders();
	// uploads data to the CPU if the shader has changed, or the end of the frame has been reached
	// bypass: uploads GPU data even if the shader hasn't changed
	static void UploadGPUData();

	static inline Shader* currShader = NULL;
	static inline Shader* prevShader = NULL;

	static inline std::unordered_map<std::string, std::unique_ptr<textureStruct>> textureMap;

	static inline std::vector<InstanceData> gpuData;

	static inline GLuint ssbo;
	static inline std::unique_ptr<VAO> vao;
	static inline std::unique_ptr<VBO> vbo;
	static inline std::unique_ptr<EBO> ebo;

	static inline GLuint samplerID = -1;
};