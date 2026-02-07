#pragma once

#include <memory>
#include <glad/glad.h>	

class Shader;
class VAO;
class VBO;
class EBO;

class Grass {
public:
	Grass();
	~Grass();

	void DrawDepth();
	void Draw(Shader* shader);
private:
	std::unique_ptr<VAO> vao;
	std::unique_ptr<VBO> vbo;
	std::unique_ptr<EBO> ebo;

	GLuint depthTex;
	GLuint fbo;

	GLuint slot = 0;
};