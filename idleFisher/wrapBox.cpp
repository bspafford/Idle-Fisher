#include "wrapBox.h"

#include "debugger.h"

UwrapBox::UwrapBox(widget* parent, vector loc, vector size) : widget(parent) {
	setLocAndSize(loc, size);
}

void UwrapBox::draw(Shader* shaderProgram) {
	for (widget* child : childrenList)
		if (child)
			child->draw(shaderProgram);
}

void UwrapBox::addChild(widget* child) {
	child->setParent(this);
	childrenList.push_back(child);
}

widget* UwrapBox::getChildAt(int index) {
	return childrenList[index];
}

std::vector<widget*> UwrapBox::getAllChildren() {
	return childrenList;
}

void UwrapBox::UpdateChildren() {
	if (childrenList.empty()) {
		overflowSizeY = 0.f;
		return;
	}

	vector initialOffset = absoluteLoc + vector{ 0.f, size.y }; // keep left and bottom aligned
	vector offset = initialOffset;

	
	vector childSize;
	for (widget* child : childrenList) {
		if (!child)
			continue;

		childSize = child->getSize();
		if (offset.x + childSize.x > initialOffset.x + size.x) { // wrap to next line
			offset.x = initialOffset.x;
			offset.y -= childSize.y + padding.y;
		}
		
		child->setLoc(offset - vector{ 0.f, childSize.y });
		offset.x += childSize.x + padding.x;
	}

	offset.y -= childSize.y; // account for last row height
	overflowSizeY = initialOffset.y - offset.y;
}

void UwrapBox::setLoc(vector loc) {
	__super::setLoc(loc);
	UpdateChildren();
}

float UwrapBox::getOverflowSize() {
	return std::abs(overflowSizeY);
}

void UwrapBox::SetPadding(vector padding) {
	this->padding = padding;
}