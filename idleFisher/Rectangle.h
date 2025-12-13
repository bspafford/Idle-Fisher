#pragma once

#include "math.h"
#include "shaderClass.h"
#include "Image.h"
#include "Hoverable.h"

class URectangle : public IHoverable {
public:
	URectangle(vector loc, vector size, bool useWorldLoc, glm::vec4 color = glm::vec4(1.f));
	~URectangle();

	void LoadGPU();

	void draw(Shader* shaderProgram);
	void setColor(glm::vec4 color);
	void setLoc(vector loc);
	vector getLoc();
	void setSize(vector size);
	vector getSize();
	void setAnchor(ImageAnchor xAnchor, ImageAnchor yAnchor);
	void updatePositionsList();

	// will stop hovers and clicks through a rectangle
	void setBlockCursor(bool val);

protected:
	bool blockCursor = false;

	vector size;
	glm::vec4 color = glm::vec4(1.f);
	bool useWorldLoc;

	vector loc; // relative to the screen position
	vector absoluteLoc; // absolute position in the screen
	ImageAnchor xAnchor = IMAGE_ANCHOR_LEFT;
	ImageAnchor yAnchor = IMAGE_ANCHOR_TOP;

	GLuint VBOId;
	VAO* currVAO = nullptr;
	EBO* currEBO = nullptr;

};