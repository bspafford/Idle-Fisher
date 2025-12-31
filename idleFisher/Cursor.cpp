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
		textureStruct* mouseImg = textureManager::getTexture("./images/cursor" + std::to_string(currCursor) + ".png");
		GLFWimage cursorImg;
		cursorImg.width = mouseImg->w;
		cursorImg.height = mouseImg->h;
		cursorImg.pixels = mouseImg->FlipBytesVertically();

		cursor = glfwCreateCursor(&cursorImg, 0, 0);
		delete[] cursorImg.pixels;
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

void Cursor::toggleCursor() {
	glfwSetCursor(Main::GetWindow(), NULL);
}