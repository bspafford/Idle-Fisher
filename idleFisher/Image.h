#pragma once

#include<glad/glad.h>

#include"shaderClass.h"
#include "math.h"
#include "Anchor.h"

class textureStruct;

class Image {
public:
	Image(std::string image, vector loc, bool useWorldPos);
	Image(std::shared_ptr<Image> image, std::shared_ptr<Rect>, vector loc, bool useWorldPos);

	~Image();

	void LoadGPU();

	// Adds to a queue that is drawn at the end of the frame or when the shader is swapped
	void draw(Shader* shader);
	// Instantly draws the Image
	void setSourceRect(std::shared_ptr<Rect> rect);
	void setLoc(vector loc);
	vector getLoc();
	vector getAbsoluteLoc();
	void setRotation(float rot);

	bool isMouseOver(bool ignoreTransparent = false);

	vector getSize();
	void setSize(vector scale);
	std::string getPath();
	void setImage(std::string path);
	void setUseAlpha(bool useAlpha);
	void setColorMod(glm::vec4 colorMod);

	glm::vec4 GetPixelColor(const int X, const int Y);

	void SetAnchor(Anchor xAnchor, Anchor yAnchor);
	void SetPivot(vector pivot);
	void flipHoizontally(bool flip);

private:
	GLuint64 handle = 0;

	bool useWorldPos;
	bool useAlpha;

	vector loc; // relative to the screen position
	vector absoluteLoc; // absolute position in the screen
	// where (0, 0) is relative to the object
	// pivot (0, 0): is bottom left, (1, 1): is top right
	vector pivot = { 0, 0 };
	Anchor xAnchor = ANCHOR_LEFT;
	Anchor yAnchor = ANCHOR_BOTTOM;

	float w = 0, h = 0;
	// scale of image without a source rect
	float ogW = 0, ogH = 0;
	std::shared_ptr<Rect> source; // 0 -> img size
	Rect normalizedSource; // 0 -> 1
	float rotation;
	std::string path;
	textureStruct* textureStructPtr = NULL;
	
	glm::vec4 colorMod = glm::vec4(1.f);

	bool flipHoriz = false;
};