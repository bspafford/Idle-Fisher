#include "verticalBox.h"

#include <iostream>

#include "debugger.h"

verticalBox::verticalBox(widget* parent) : widget(parent) {

}

void verticalBox::draw(Shader* shaderProgram) {
	float yOffset = loc.y;
	for (vertChildComp comp : childList) {
		// need to update position depending on index in horizontal box
		if (comp.child)
			comp.child->draw(shaderProgram);
		yOffset -= comp.widgetHeight;
	}

	overflowSizeY = yOffset - loc.y;
}

void verticalBox::addChild(widget* child, float widgetHeight) {
	vertChildComp comp = { child, widgetHeight };
	if (comp.child)
		comp.child->setParent(this);
	childList.push_back(comp);
}

void verticalBox::removeChild(widget* child) {

}

void verticalBox::removeChild(int index) {
	childList.erase(childList.begin() + index);
}

float verticalBox::getOverflowSize() {
	// if the overflow size isn't set yet
	if (overflowSizeY == 0) {
		float yOffset = 0;
		for (vertChildComp comp : childList) {
			yOffset -= comp.widgetHeight;
		}

		overflowSizeY = yOffset;
	}

	return overflowSizeY;
}

void verticalBox::changeChildHeight(widget* child, float newHeight) {
	for (vertChildComp& comp : childList) {
		if (comp.child == child) {
			comp.widgetHeight = newHeight;
			break;
		}
	}

	float yOffset = loc.y;
	for (vertChildComp comp : childList) {
		if (comp.child)
			comp.child->setLocAndSize(vector{ loc.x, yOffset }, vector{ size.x, comp.widgetHeight });
		yOffset -= comp.widgetHeight;
	}
	overflowSizeY = yOffset - loc.y;
}

void verticalBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc, size);

	float yOffset = loc.y;
	for (vertChildComp comp : childList) {
		// need to update position depending on index in horizontal box
		// comp.child->draw(shaderProgram);
		if (comp.child)
			comp.child->setLocAndSize({ loc.x, yOffset }, { size.x, comp.widgetHeight });
		yOffset -= comp.widgetHeight;
	}

	overflowSizeY = yOffset - loc.y;
}

void verticalBox::setOgLoc(vector ogLoc) {
	__super::setOgLoc(ogLoc);

	for (vertChildComp comp : childList)
		if (comp.child)
			comp.child->setOgLoc(ogLoc);
}
