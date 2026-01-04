#include "scrollBox.h"
#include "Input.h"
#include "ScissorTest.h"

#include "debugger.h"

void UscrollBox::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	bool isMouseOver = mouseOver();
	if (Input::getMouseButtonDown(MOUSE_BUTTON_RIGHT) && isMouseOver) {
		scrollingActive = true;
	} else if (Input::getMouseButtonUp(MOUSE_BUTTON_RIGHT)) {
		scrollingActive = false;
	}

	bool isRightMouseHeld = Input::getMouseButtonHeld(MOUSE_BUTTON_RIGHT);

	if (scrollingActive) {
		IHoverable::setHoveredItem(this);
		if (Input::getMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			startLoc = absoluteLoc;
			mouseStartPos = Input::getMousePos();
		}

		if (isRightMouseHeld) {
			setCursorHoverIcon(CURSOR_GRAB);
			scrolling();
		}
	}

	if (!isRightMouseHeld && isMouseOver) {
		scrolling(Input::getMouseScrollDir());
		setCursorHoverIcon(CURSOR_DEFAULT);
	}

	ScissorTest::Enable(ogLoc, size);
	
	__super::draw(shaderProgram);

	ScissorTest::Disable();
}

// mouse right click scrolling
void UscrollBox::scrolling() {
	if (overflowSizeY < size.y) {
		absoluteLoc.y = -overflowSizeY + size.y;
		return;
	}

	scrollingActive = true;

	float diff = mouseStartPos.y - startLoc.y;
	absoluteLoc.y = clampY(Input::getMousePos().y - diff);

	UpdateChildren();
}


void UscrollBox::scrolling(int mouseWheelDir) {
	if (mouseWheelDir == 0)
		return;

	if (overflowSizeY < size.y) {
		absoluteLoc.y = -overflowSizeY + size.y;
		return;
	}

	float diff = -mouseWheelDir * 10.f + absoluteLoc.y;
	absoluteLoc.y = clampY(diff);

	UpdateChildren();
}

bool UscrollBox::mouseOver() {
	vector mousePos = Input::getMousePos();
	if (mousePos.x >= ogLoc.x && mousePos.x <= ogLoc.x + size.x && mousePos.y >= ogLoc.y && mousePos.y <= ogLoc.y + size.y)
		return true;
	return false;
}

float UscrollBox::clampY(float y) {
	return math::clamp(y, ogLoc.y, ogLoc.y + overflowSizeY - size.y);
}