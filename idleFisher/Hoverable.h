#pragma once

#include <string>

#include "Cursor.h"

class IHoverable {
public:
	virtual ~IHoverable();

	// set what the mouse hover icon should be
	void setMouseHoverIcon(CursorType cursorName);
	CursorType getMouseHoverIcon();

	static IHoverable* getHoveredItem();
	static void setHoveredItem(IHoverable* item);
	// checks to see if hoveredItem is a widget
	// if it is a widget then it will see if it's part of the same tree that the currWidget is in
	// if it is part of the same tree then the funciton will return true, if not then it will return false
	// if hoveredItem is not a widget, then it will return true
	static bool checkValidInteract();

private:
	// which mouse should be active on hover
	CursorType currCursor = CURSOR_POINT;

	static inline IHoverable* hoveredItem;
};