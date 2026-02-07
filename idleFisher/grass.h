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

struct GrassInstanceData {
	glm::vec2 position;
	glm::vec3 color;
	float inShadow;
};

class Grass {
public:
	Grass();
	void LoadGPU();
	void DrawDepth();
	void Draw();
	void ResizeScreen();

private:
	std::vector<std::vector<glm::vec2>> GetGrassPolygons();
	std::vector<std::vector<glm::vec2>> GetShadowPolygons();

	std::vector<GrassInstanceData> grassData;

	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	std::unique_ptr<EBO> ebo;

	std::unique_ptr<FBO> fbo;

	// used for locs and color data
	std::unique_ptr<VBO> grassDataVBO;
	int grassNum = 0;

	glm::vec3 grassColor1 = glm::vec3(114.0 / 255.0, 145.0 / 255.0, 40.0 / 255.0);
	glm::vec3 grassColor2 = glm::vec3(107.0 / 255.0, 132.0 / 255.0, 45.0 / 255.0);
	glm::vec3 grassColor3 = glm::vec3(97.0 / 255.0, 122.0 / 255.0, 44.0 / 255.0);
	glm::vec3 grassHighlight1 = glm::vec3(120.0 / 255.0, 158.0 / 255.0, 36.0 / 255.0);
	glm::vec3 grassHighlight2 = glm::vec3(88.0 / 255.0, 113.0 / 255.0, 44.0 / 255.0);
};