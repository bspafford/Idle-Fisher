#include "grass.h"
#include "Texture.h"
#include "Scene.h"

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "FBO.h"
#include "Rectangle.h"

Grass::Grass() {
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/grass.png", "grass");
	Texture::bindTextureToShader(Scene::grassShader, "images/worlds/demo/tallGrass.png", "tallGrass");

	Scene::grassShader->Activate();
	Scene::grassShader->setVec3("grassColor1", glm::vec3(68.0 / 255.0, 113.0 / 255.0, 25.0 / 255.0));
	Scene::grassShader->setVec3("grassColor2", glm::vec3(83.0 / 255.0, 120.0 / 255.0, 23.0 / 255.0));
	Scene::grassShader->setVec3("grassColor3", glm::vec3(56.0 / 255.0, 107.0 / 255.0, 4.0 / 255.0));
	Scene::grassShader->setVec3("grassHighlight1", glm::vec3(57.0 / 255.0, 99.0 / 255.0, 5.0 / 255.0));
	Scene::grassShader->setVec3("grassHighlight2", glm::vec3(96.0 / 255.0, 136.0 / 255.0, 9.0 / 255.0));

	float vertices[] = {
		// positions       // texture coordinates
		0.0f, 1.0f,      0.0f, 0.0f,  // top-left
		0.0f, 0.0f,      0.0f, 1.0f,  // bottom-left
		1.0f, 0.0f,      1.0f, 1.0f,  // bottom-right
		1.0f, 1.0f,      1.0f, 0.0f   // top-right
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	vao = std::make_unique<VAO>();
	vao->Bind();
	vbo = std::make_unique<VBO>(vertices, sizeof(vertices));
	ebo = std::make_unique<EBO>(indices, sizeof(indices));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);

	fbo = std::make_unique<FBO>(stuff::screenSize, true, FBOType::DepthOnly);
}

void Grass::DrawDepth() {
	fbo->Bind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);   // WRITE depth

	Scene::grassShader->Activate();

	// draw grass
	vao->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 5000);

	// unbind fbo
	fbo->Unbind();
}

void Grass::Draw() {
	DrawDepth();

	Scene::grassShader->Activate();
	// draw grass
	vao->Bind();
	
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glDepthMask(GL_TRUE);

	Scene::grassShader->setInt("isGround", 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	Scene::grassShader->setInt("isGround", 0);

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 5000);

	glDisable(GL_DEPTH_TEST);

	Scene::twoDShader->Activate();
	Texture* tex = fbo->GetDepthTexture();
	tex->Bind();
	tex->texUnit(Scene::twoDShader, "grassDepthTex");
}