#pragma once

#include "widget.h"
#include "shaderClass.h"
#include "FBO.h"

class Texture;

class BlurBox : public widget {
public:
	BlurBox(widget* parent, vector loc, vector size, int blurStrength);
	~BlurBox();
	
	void draw();
	void GenerateSubTexture(Texture* texture);

	static void Init();
	static void DrawFinal(Shader* shader);
	static void BindFramebuffer();
	static void UnbindFramebuffer();
	static void ResizeScreen();
	virtual void setLoc(vector loc) override;
	virtual void setSize(vector size) override;
private:
	std::unique_ptr<Texture> texture;
	int blurStrength = 5;
	bool useWorldPos = false;

	static inline std::vector<BlurBox*> instances;
	static inline std::unique_ptr<FBO> sceneFBO;
};