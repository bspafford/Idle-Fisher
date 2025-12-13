#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include <stb/stb_image.h>
#include <vector>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

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
	GLuint ID;
	GLenum type;
	Image(std::string image, vector loc, bool useWorldPos);
	Image(std::shared_ptr<Image> image, std::shared_ptr<Rect>, vector loc, bool useWorldPos);
	~Image();

	void LoadGPU();

	void draw(Shader* shaderProgram);
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

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	bool useWorldPos;
	bool useAlpha;

	float w = 0, h = 0;
	// scale of image without a source rect
	float ogW = 0, ogH = 0;
	std::shared_ptr<Rect> source;
	float rotation;
	unsigned char* texture;
	GLenum textureFormat;
	std::string path;

	void updatePositionsList(std::vector<float> positions = std::vector<float>(0));
private:
	std::vector<float> getPositionsList();

	GLenum texType = GL_TEXTURE_2D;
	GLenum pixelType = GL_UNSIGNED_BYTE;

	std::unique_ptr<VAO> currVAO = nullptr;
	GLuint VBOId = 0;
	std::unique_ptr<EBO> currEBO = nullptr;

	std::vector<float> positions;

	vector loc; // relative to the screen position
	vector absoluteLoc; // absolute position in the screen
	ImageAnchor xAnchor = IMAGE_ANCHOR_LEFT;
	ImageAnchor yAnchor = IMAGE_ANCHOR_TOP;
	
	glm::vec4 colorMod = glm::vec4(1.f);

	bool flipHoriz = false;

	bool calledLoadGPU = false;
};
#endif