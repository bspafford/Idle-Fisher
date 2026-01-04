#pragma once

#include "widget.h"
#include "Hoverable.h"
#include "math.h"

class URectangle : public widget, public IHoverable {
public:
	URectangle(widget* parent, vector loc, vector size, bool useWorldLoc, glm::vec4 color = glm::vec4(1.f));

	void draw(Shader* shaderProgram);
	void setColor(glm::vec4 color);

	// will stop hovers and clicks through a rectangle
	void setBlockCursor(bool val);

protected:
	bool blockCursor = false;

	glm::vec4 color = glm::vec4(1.f);
	bool useWorldLoc;
};