#pragma once

#include "widget.h"
#include "shaderClass.h"

class BlurBox : public widget {
public:
	BlurBox(widget* parent, vector loc, vector size, int blurStrength);
	~BlurBox();

	void draw(Shader* shader);
	void GenerateSubTexture(GLuint texture);

	static void Init();
	static void DrawFinal();
	static void BindFrameBuffer();
	static void UnbindFrameBuffer();
	static void BindTexture();
	static void UnbindTexture();
	static void BindShader();
	static void OnReizeWindow();
	virtual void setLoc(vector loc) override;
	virtual void setSize(vector size) override;
private:
	static inline std::vector<BlurBox*> instances;

	void makeVerticesList();
	void UpdateVertices();
	virtual void setupLocs() override;
	
	int blurStrength = 5;

	GLuint texture;
	GLuint quadVAO, quadVBO, quadEBO;

	std::vector<float> quadVertices;

	static inline std::unique_ptr<Shader> blurShader;
	static inline GLuint sceneFBO;
	static inline GLint prevBoundFBO;
	static inline GLuint sceneTexture;
	static inline GLuint sceneVAO, sceneVBO, sceneEBO;

	static inline float sceneVertices[] = {
		// positions       // texture coordinates
		-1.0f,  1.0f,      0.0f, 1.0f,  // top-left
		-1.0f, -1.0f,      0.0f, 0.0f,  // bottom-left
		 1.0f, -1.0f,      1.0f, 0.0f,  // bottom-right
		 1.0f,  1.0f,      1.0f, 1.0f   // top-right
	};

	static inline unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3
	};
};