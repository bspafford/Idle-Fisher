#include "Rectangle.h"
#include "input.h"
#include "textureManager.h"

#include "debugger.h"

URectangle::URectangle(widget* parent, vector loc, vector size, bool useWorldLoc, glm::vec4 color) : widget(parent) {
	this->useWorldLoc = useWorldLoc;
	this->color = color;
	setLocAndSize(loc, size);

	setCursorHoverIcon(CURSOR_DEFAULT);
}

void URectangle::draw(Shader* shaderProgram) {
	vector mousePos = Input::getMousePos();
	if (blockCursor && mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		setHoveredItem(this);

	textureManager::DrawRect(shaderProgram, absoluteLoc, size, useWorldLoc, color, true);
}

void URectangle::setColor(glm::vec4 color) {
	this->color = color;
}

void URectangle::setBlockCursor(bool val) {
	blockCursor = val;
}