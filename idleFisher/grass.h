#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "FBO.h"

class Shader;
class VAO;
class VBO;
class EBO;
class URectangle;

class Grass {
public:
	Grass();

	void DrawDepth();
	void Draw();
	void ResizeScreen();

private:
	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	std::unique_ptr<EBO> ebo;

	std::unique_ptr<FBO> fbo;

	// used for locs and color data
	std::unique_ptr<VBO> grassDataVBO;
	int grassNum = 0;
};