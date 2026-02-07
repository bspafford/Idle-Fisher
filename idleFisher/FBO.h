#pragma once

#include <stack>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "math.h"

class Shader;
class Texture;

enum class FBOType {
	ColorOnly,
	DepthOnly,
	ColorAndDepth,
};

struct FBOData {
	GLuint ID = -1;
	vector fboSize;

	FBOData(GLuint _id, vector _fboSize) : ID(_id), fboSize(_fboSize) {}
};

class FBO {
public:
	FBO(vector size, bool useWorldPos, FBOType type);
	~FBO();

	void Draw(Shader* shader, const vector& position, const Rect& source, const bool& useWorldPos, const glm::vec4& color);

	void ResizeTexture(vector size);

	vector GetSize();

	void Bind(glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));
	void Unbind();

	Texture* GetColorTexture();
	Texture* GetDepthTexture();

	static FBOData GetCurrFBO();

private:
	GLuint ID = 0;
	std::unique_ptr<Texture> texture;
	std::unique_ptr<Texture> depthTexture;
	vector size;
	FBOType type;

	// keeps track of what FBO the program is in
	// along with what FBO that FBO was in
	static inline std::stack<FBOData> fboStack;
};