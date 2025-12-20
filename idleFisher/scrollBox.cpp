#include "scrollBox.h"
#include "Input.h"

#include "debugger.h"

UscrollBox::UscrollBox(widget* parent) : verticalBox(parent) {
}

void UscrollBox::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	if (mouseOver()) {
		IHoverable::setHoveredItem(this);
		if (Input::getMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			startLoc = loc;
			mouseStartPos = Input::getMousePos();
		}
		if (Input::getMouseButtonHeld(MOUSE_BUTTON_RIGHT)) {
			setCursorHoverIcon(CURSOR_GRAB);
			scrolling();
		} else {
			scrolling(Input::getMouseScrollDir());
			setCursorHoverIcon(CURSOR_DEFAULT);
		}
	}

	glEnable(GL_SCISSOR_TEST);
	glScissor(ogLoc.x, stuff::screenSize.y - size.y - ogLoc.y, size.x, size.y);

	__super::draw(shaderProgram);

	glDisable(GL_SCISSOR_TEST);
}

// mouse right click scrolling
void UscrollBox::scrolling() {
	if (overflowSizeY < size.y) {
		loc.y = -overflowSizeY + size.y;
		return;
	}

	vector diff = mouseStartPos - startLoc;
	loc.y = math::clamp(Input::getMousePos().y - diff.y, -overflowSizeY + size.y, 0);

	// need to set location of all the children
	float yOffset = 0;
	for (vertChildComp comp : childrenList) {
		if (comp.child)
			comp.child->setLoc(ogLoc + vector{ 0, loc.y + yOffset });
		yOffset += comp.widgetHeight;
	}
}

void UscrollBox::scrolling(int mouseWheelDir) {
	if (mouseWheelDir == 0)
		return;

	if (overflowSizeY < size.y) {
		loc.y = -overflowSizeY + size.y;
		return;
	}

	vector diff = vector{ 0, mouseWheelDir * 10.f * stuff::pixelSize } + loc;
	loc.y = math::clamp(diff.y, -overflowSizeY + size.y, 0);

	float yOffset = 0;
	for (vertChildComp comp : childrenList) {
		if (comp.child)
			comp.child->setLocAndSize({ ogLoc.x, ogLoc.y + loc.y + yOffset }, comp.child->getSize());
		yOffset += comp.widgetHeight;
	}
}

bool UscrollBox::mouseOver() {
	vector mousePos = Input::getMousePos();
	if (mousePos.x >= ogLoc.x && mousePos.x <= ogLoc.x + size.x && mousePos.y >= ogLoc.y && mousePos.y <= ogLoc.y + size.y)
		return true;
	return false;
}

vector UscrollBox::getLoc() {
	return ogLoc;
}

void UscrollBox::setLoc(vector loc) {
	ogLoc = loc;
}

void UscrollBox::setLocAndSize(vector loc, vector size) {
	setLoc(loc);
	setSize(size);

	float yOffset = 0;
	for (vertChildComp comp : childrenList) {
		// need to update position depending on index in horizontal box
		if (comp.child)
			comp.child->setLocAndSize({ ogLoc.x, ogLoc.y + yOffset }, { comp.child->getSize().x, comp.widgetHeight});
		yOffset += comp.widgetHeight;
	}

	overflowSizeY = yOffset;
}