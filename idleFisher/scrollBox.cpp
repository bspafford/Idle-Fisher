#include "scrollBox.h"
#include "Input.h"
#include "ScissorTest.h"

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

	//glScissor(ogLoc.x, ogLoc.y, size.x, size.y);
	ScissorTest::Enable(ogLoc, size);
	
	__super::draw(shaderProgram);

	ScissorTest::Disable();
}

// mouse right click scrolling
void UscrollBox::scrolling() {
	if (overflowSizeY < size.y) {
		loc.y = -overflowSizeY + size.y;
		return;
	}

	vector diff = mouseStartPos - startLoc;
	loc.y = math::clamp(Input::getMousePos().y - diff.y, ogLoc.y, overflowSizeY - size.y);

	UpdateChildren();
}


void UscrollBox::scrolling(int mouseWheelDir) {
	if (mouseWheelDir == 0)
		return;

	if (overflowSizeY < size.y) {
		loc.y = -overflowSizeY + size.y;
		return;
	}

	vector diff = vector{ 0, -mouseWheelDir * 10.f } + loc;
	loc.y = math::clamp(diff.y, ogLoc.y, overflowSizeY - size.y);

	UpdateChildren();
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
	__super::setLoc(loc);
	absoluteLoc = GetAbsoluteLoc(loc, size, false, pivot, xAnchor, yAnchor);
	ogLoc = absoluteLoc;
}

void UscrollBox::setLocAndSize(vector loc, vector size) {
	setLoc(loc);
	setSize(size);

	UpdateChildren();
}