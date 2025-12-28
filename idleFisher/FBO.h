#pragma once

#include <stack>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "math.h"

class Shader;
class Texture;

struct FBOData {
	GLuint ID = -1;
	vector fboSize;

	FBOData(GLuint _id, vector _fboSize) : ID(_id), fboSize(_fboSize) {}
};

class FBO {
public:
	FBO(vector size, bool useWorldPos);
	~FBO();

	void Draw(Shader* shader, const vector& position, const Rect& source, const bool& useWorldPos, const glm::vec4& color);
	vector GetSize();

	void Bind(glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));
	void Unbind();

	static GLuint GetCurrFBO();

private:
	GLuint ID = 0;
	std::unique_ptr<Texture> texture;
	vector size;

	// keeps track of what FBO the program is in
	// along with what FBO that FBO was in
	static inline std::stack<FBOData> fboStack;
};