#pragma once

#include "math.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

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
	GLuint id = 0;
	GLuint64 handle = 0;
	int w = 0;
	int h = 0;
	int nChannels = 0;
	uint16_t keptData = 0;
	std::vector<uint8_t> bytes;

	// keepData: what pixel data to keep in CPU memory: 
	//	GL_ALPHA for only alpha
	//	GL_RGBA for full data
	//	0 for none
	textureStruct(std::vector<uint8_t>& bytes, int w, int h, int nChannels, uint16_t keepData);
	textureStruct(uint32_t hashedId);
	~textureStruct();
	
	void LoadGPU();

	bool GetAlphaAtPos(vector pos);

	// returns a copy of the flipped bytes
	// so it doesn't continue to flip back and forth
	// don't just flip once loadGPU is called because images can query before then
	std::vector<uint8_t> GetFlippedBytes();
};

class textureManager {
public:
	// creates objects for GPU
	textureManager();
	// loads all of the images
	static void LoadTextures();
	static void Deconstructor();
	// retreives texture from the texture map or loads it if there wasn't one in the map
	static textureStruct* getTexture(const std::string& name);

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

	// returns true if already called LoadTextures() function
	static bool GetTexturesLoaded();

	// GL_NEAREST, GL_LINEAR
	static void SetInterpMethod(int method);

private:
	// loads in the texture from memory and adds it to a texture map
	// assumes caller has already checked if texture is in textureMap
	static textureStruct* loadTexture(uint32_t hashedId);
	// checks if the shader has changed
	static void CheckShaders();
	// uploads data to the CPU if the shader has changed, or the end of the frame has been reached
	// bypass: uploads GPU data even if the shader hasn't changed
	static void UploadGPUData();

	static inline Shader* currShader = NULL;
	static inline Shader* prevShader = NULL;

	static inline std::unordered_map<uint32_t, std::unique_ptr<textureStruct>> textureMap;

	static inline std::vector<InstanceData> gpuData;

	static inline GLuint ssbo;
	static inline std::unique_ptr<VAO> vao;
	static inline std::unique_ptr<VBO> vbo;
	static inline std::unique_ptr<EBO> ebo;

	static inline GLuint samplerID = -1;

	static inline bool areTexturesLoaded = false;

	static inline std::recursive_mutex mutex;
};