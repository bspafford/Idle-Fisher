#include "Hoverable.h"

#include "widget.h"

IHoverable::~IHoverable() {
	if (hoveredItem == this)
		hoveredItem = nullptr;
}

// set what the mouse hover icon should be
void IHoverable::setMouseHoverIcon(CursorType cursorName) {
	currCursor = cursorName;
}

CursorType IHoverable::getMouseHoverIcon() {
	return currCursor;
}

IHoverable* IHoverable::getHoveredItem() {
	return hoveredItem;
}

void IHoverable::setHoveredItem(IHoverable* item) {
	hoveredItem = item;
}

bool IHoverable::checkValidInteract() {
	if (!hoveredItem)
		return false;
	// if object is widget, then check if its part of the override widget
	// if no overriding widget left click
	widget* _widget = dynamic_cast<widget*>(hoveredItem);
	if (!widget::getCurrWidget() || (_widget && _widget->getRootParent() == widget::getCurrWidget()))
		return true;
	return false;
}