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

struct FBOData {
	GLuint ID = 0;
	glm::vec4 fboSize = glm::vec4(0);

	FBOData(GLuint _id, glm::vec4 _fboSize) : ID(_id), fboSize(_fboSize) {}
};

struct textureStruct {
	// if useAlpha == true then if mouse over a == 0 then it won't trigger
	GLuint64 handle;
	bool useAlpha = false;
	int w = 0;
	int h = 0;
	int nChannels;

	textureStruct(const std::string& path, bool _useAlpha);
	~textureStruct() {}

private:
};

class textureManager {
public:
	textureManager();
	static void Deconstructor();
	static textureStruct* loadTexture(std::string path, bool loadSurface = false);
	static textureStruct* getTexture(std::string name);

	static void StartFrame();
	// Adds the Image into a draw queue to render at end of frame or when shader is swapped
	static void DrawImage(Shader* shader, const vector& position, const vector& size, const Rect& source, const bool& useWorldPos, const glm::vec4& color, const uint64_t& tex);
	// Instantly draws the image, used for things like FBOs
	static void DrawRect(Shader* shader, const vector& position, const vector& size, const bool& useWorldPos, const glm::vec4& color);
	// uploads data to the CPU if the shader has changed, or the end of the frame has been reached
	// bypass: uploads GPU data even if the shader hasn't changed
	static void UploadGPUData(Shader* shader, bool bypass = false);

	static void BindFramebuffer(GLuint ID, glm::vec4 fboSize, glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));
	static void UnbindFramebuffer();

	static GLuint GetSamplerID();
	static GLuint GetCurrFBO();

private:
	static inline Shader* currShader = NULL;
	static inline Shader* prevShader = NULL;

	static inline std::unordered_map<std::string, std::unique_ptr<textureStruct>> textureMap;

	static inline std::vector<InstanceData> gpuData;

	static inline GLuint ssbo;
	static inline std::unique_ptr<VAO> vao;
	static inline std::unique_ptr<VBO> vbo;
	static inline std::unique_ptr<EBO> ebo;

	static inline GLuint samplerID = -1;

	// keeps track of what FBO the program is in
	// along with what FBO that FBO was in
	static inline std::stack<FBOData> fboStack;
};