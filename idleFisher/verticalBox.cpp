#include "verticalBox.h"

#include <iostream>

#include "debugger.h"

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

void verticalBox::RemoveAllChildren() {
	overflowSizeY = 0.f;
	childrenList.clear();
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
	__super::setLocAndSize(loc, size);

	UpdateChildren();
}

void verticalBox::UpdateChildren() {
	float initialOffset = absoluteLoc.y + size.y;
	float yOffset = initialOffset;

	for (vertChildComp comp : childrenList) {
		// need to update position depending on index in horizontal box
		if (comp.child)
			comp.child->setLocAndSize({ absoluteLoc.x, yOffset - comp.widgetHeight }, { size.x, comp.widgetHeight });
		yOffset -= comp.widgetHeight;
	}

	overflowSizeY = initialOffset - yOffset;
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
