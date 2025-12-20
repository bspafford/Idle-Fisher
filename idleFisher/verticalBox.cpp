#include "verticalBox.h"

#include <iostream>

#include "debugger.h"

verticalBox::verticalBox(widget* parent) : widget(parent) {

}

void verticalBox::draw(Shader* shaderProgram) {
	for (vertChildComp comp : childrenList) {
		if (comp.child)
			comp.child->draw(shaderProgram);
	}
}

void verticalBox::addChild(widget* child, float widgetHeight) {
	vertChildComp comp = { child, widgetHeight };
	if (comp.child)
		comp.child->setParent(this);
	childrenList.push_back(comp);

	overflowSizeY += widgetHeight;
}

void verticalBox::removeChild(widget* child) {

}

void verticalBox::removeChild(int index) {
	overflowSizeY -= childrenList[index].widgetHeight;
	childrenList.erase(childrenList.begin() + index);
}

float verticalBox::getOverflowSize() {
	return std::abs(overflowSizeY);
}

void verticalBox::changeChildHeight(widget* child, float newHeight) {
	for (vertChildComp& comp : childrenList) {
		if (comp.child == child) {
			comp.widgetHeight = newHeight;
			break;
		}
	}

	UpdateChildren();
}

void verticalBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc * stuff::pixelSize, size);

	UpdateChildren();
}

void verticalBox::UpdateChildren() {
	float yOffset = absoluteLoc.y + size.y * pivot.y;
	for (vertChildComp comp : childrenList) {
		// need to update position depending on index in horizontal box
		// comp.child->draw(shaderProgram);
		if (comp.child)
			comp.child->setLocAndSize({ absoluteLoc.x, yOffset - comp.widgetHeight * pivot.y }, { size.x, comp.widgetHeight });
		yOffset -= comp.widgetHeight;
	}

	overflowSizeY = yOffset - absoluteLoc.y;
}

void verticalBox::setOgLoc(vector ogLoc) {
	__super::setOgLoc(ogLoc);

	for (vertChildComp comp : childrenList)
		if (comp.child)
			comp.child->setOgLoc(ogLoc);
}

vertChildComp verticalBox::GetChildAt(int index) {
	return childrenList[index];
}

int verticalBox::GetChildrenCount() {
	return static_cast<int>(childrenList.size());
}
