#include "cursor.h"
#include "main.h"
#include "Input.h"
#include "Hoverable.h"
#include "textureManager.h"

#include "debugger.h"

void Cursor::calcMouseImg() {
	IHoverable* hoveredItem = IHoverable::getHoveredItem();
	bool canHover = IHoverable::checkValidInteract();
	if (hoveredItem && canHover) {
		CursorType hoveredIcon = hoveredItem->getCursorHoverIcon();
		if (Input::getMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT)) {
			if (hoveredIcon == CURSOR_POINT)
				setCursorIcon(CURSOR_CLICK);
			else if (hoveredIcon == CURSOR_HOVER)
				setCursorIcon(CURSOR_GRAB);
		} else
			setCursorIcon(hoveredItem->getCursorHoverIcon());
	} else if (!hoveredItem && mouseOverWater && !widget::getCurrWidget())
		setCursorIcon(CURSOR_HOOK);
	else if (!hoveredItem || !canHover)
		setCursorIcon(CURSOR_DEFAULT);

	// resets hoveredItem for next frame
	IHoverable::setHoveredItem(nullptr);
}

void Cursor::setCursorIcon(CursorType cursorName) {
	// return if same cursor
	if (currCursor == cursorName)
		return;

	currCursor = cursorName;

	if (SaveData::settingsData.cursor) {
		// should be getting texture from texture manager not loading them ever time i switch
		textureStruct* mouseImg = textureManager::getTexture("images/cursor" + std::to_string(currCursor) + ".png");
		std::vector<uint8_t> flipped = mouseImg->GetFlippedBytes();

		GLFWimage cursorImg;
		cursorImg.width = mouseImg->w;
		cursorImg.height = mouseImg->h;
		cursorImg.pixels = flipped.data();

		cursor = glfwCreateCursor(&cursorImg, 0, 0);

		if (cursor)
			glfwSetCursor(Main::GetWindow(), cursor);
	} else {
		GLFWcursor* handCursor = glfwCreateStandardCursor(cursorLookup[cursorName]);
		glfwSetCursor(Main::GetWindow(), handCursor);
	}
}

bool Cursor::getMouseOverWater() {
	return mouseOverWater;
}

void Cursor::setMouseOverWater(bool overWater) {
	mouseOverWater = overWater;
}

bool Cursor::GetMouseOverRiver() {
	return mouseOverRiver;
}

void Cursor::SetMouseOverRiver(bool overRiver) {
	mouseOverRiver = overRiver;
}

void Cursor::toggleCursor() {
	glfwSetCursor(Main::GetWindow(), NULL);
}