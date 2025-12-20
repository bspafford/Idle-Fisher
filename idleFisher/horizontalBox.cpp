#include "horizontalBox.h"

#include <iostream>

#include "debugger.h"

horizontalBox::horizontalBox(widget* parent) : widget(parent) {

}

void horizontalBox::draw(Shader* shaderProgram) {
	float xOffset = loc.x;
	for (horizChildComp comp : childList) {
		// need to update position depending on index in horizontal box
		if (comp.child)
			comp.child->draw(shaderProgram);
		//comp.child->setLocAndSize({ loc.x, xOffset }, { size.x, comp.widgetWidth });
		xOffset += comp.widgetWidth;
	}

	overflowSizeX = xOffset - loc.x;
}

void horizontalBox::addChild(widget* child, float widgetWidth) {
	horizChildComp comp = { child, widgetWidth };
	if (comp.child) {
		comp.child->setParent(this);
		comp.child->setOgLoc(ogLoc);
	}
	childList.push_back(comp);
}

void horizontalBox::removeChild(widget* child) {
	/*
	// could be a problem
	auto it = std::find(childList.begin(), childList.end(), child);

	// checks if child is in list
	if (it != childList.end())
		childList.erase(it);
		*/
}

void horizontalBox::removeChild(int index) {
	childList.erase(childList.begin() + index);
}

float horizontalBox::getOverflowSize() {
	// if the overflow size isn't set yet
	if (overflowSizeX == 0) {
		float xOffset = 0;
		for (horizChildComp comp : childList) {
			xOffset += comp.widgetWidth;
		}

		overflowSizeX = xOffset;
	}

	return overflowSizeX;
}

void horizontalBox::changeChildWidth(widget* child, float newWidth) {
	for (horizChildComp& comp : childList) {
		if (comp.child == child) {
			comp.widgetWidth = newWidth;
			break;
		}
	}

	float xOffset = loc.x;
	for (horizChildComp comp : childList) {
		if (comp.child)
			comp.child->setLocAndSize({ xOffset, loc.y }, { comp.widgetWidth, size.y });
		xOffset += comp.widgetWidth;
	}

	overflowSizeX = xOffset - loc.x;
}

void horizontalBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc, size);

	float xOffset = loc.x;
	for (horizChildComp comp : childList) {
		// need to update position depending on index in horizontal box
		// comp.child->draw(shaderProgram);
		if (comp.child)
			comp.child->setLocAndSize({ xOffset, loc.y }, { comp.widgetWidth, size.y });
		xOffset += comp.widgetWidth;
	}

	overflowSizeX = xOffset - loc.x;
}

void horizontalBox::setOgLoc(vector ogLoc) {
	__super::setOgLoc(ogLoc);

	for (horizChildComp comp : childList)
		if (comp.child)
			comp.child->setOgLoc(ogLoc);
}