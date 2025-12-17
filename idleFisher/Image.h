#pragma once

#include<glad/glad.h>

#include"shaderClass.h"
#include "math.h"

enum ImageAnchor {
	IMAGE_ANCHOR_LEFT,
	IMAGE_ANCHOR_RIGHT,
	IMAGE_ANCHOR_TOP,
	IMAGE_ANCHOR_BOTTOM,
	IMAGE_ANCHOR_CENTER
};

class Image {
public:
	Image(std::string image, vector loc, bool useWorldPos);
	Image(std::shared_ptr<Image> image, std::shared_ptr<Rect>, vector loc, bool useWorldPos);

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
	void setImage(std::string path);
	void setUseAlpha(bool useAlpha);
	void setColorMod(glm::vec4 colorMod);

	glm::vec4 GetPixelColor(const int X, const int Y);

	void setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor);
	void flipHoizontally(bool flip);

	bool useWorldPos;
	bool useAlpha;

	float w = 0, h = 0;
	// scale of image without a source rect
	float ogW = 0, ogH = 0;
	std::shared_ptr<Rect> source; // 0 -> img size
	Rect normalizedSource; // 0 -> 1
	float rotation;
	std::string path;

private:
	GLuint64 handle = 0;

	vector loc; // relative to the screen position
	vector absoluteLoc; // absolute position in the screen
	ImageAnchor xAnchor = IMAGE_ANCHOR_LEFT;
	ImageAnchor yAnchor = IMAGE_ANCHOR_BOTTOM;
	
	glm::vec4 colorMod = glm::vec4(1.f);

	bool flipHoriz = false;
};