#include "Rectangle.h"
#include "stuff.h"
#include "input.h"
#include "textureManager.h"

#include "debugger.h"

URectangle::URectangle(vector loc, vector size, bool useWorldLoc, glm::vec4 color) {
	this->loc = loc;
	this->size = size;
	this->color = color;
	this->useWorldLoc = useWorldLoc;

	setCursorHoverIcon(CURSOR_DEFAULT);

	setLoc(loc);
}

URectangle::~URectangle() {

}

void URectangle::draw(Shader* shaderProgram) {
	vector mousePos = Input::getMousePos();
	if (blockCursor && mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		setHoveredItem(this);

	textureManager::DrawRect(shaderProgram, absoluteLoc, size, useWorldLoc, color);
}

void URectangle::setColor(glm::vec4 color) {
	this->color = color;
}

void URectangle::setLoc(vector loc) {
	this->loc = loc;
	absoluteLoc = GetAbsoluteLoc(loc, size, useWorldLoc, pivot, xAnchor, yAnchor);
}

vector URectangle::getLoc() {
	return loc;
}

void URectangle::setSize(vector size) {
	this->size = size;
}

vector URectangle::getSize() {
	return size;
}

void URectangle::setAnchor(Anchor xAnchor, Anchor yAnchor) {
	if (useWorldLoc) {
		std::cout << "This is a world object, it doesn't work";
		return;
	}

	this->xAnchor = xAnchor;
	this->yAnchor = yAnchor;
	setLoc(loc);
}

void URectangle::SetPivot(vector pivot) {
	this->pivot = pivot;
	setLoc(loc);
}

void URectangle::setBlockCursor(bool val) {
	blockCursor = val;
}