#include "widget.h"
#include <iostream>
#include "main.h"
#include "Input.h"
#include "character.h"
#include "Image.h"
#include "shaderClass.h"

// temp
#include "worlds.h"
#include "scuba.h"

#include "debugger.h"

widget::widget(widget* parent) {
	instances.insert(this);

	this->parent = parent;
	// add this as parents child

	if (parent) {
		parent->children.push_back(this);
		setRootParent(parent->getRootParent());
	} else // if parent null, this is the root
		setRootParent(this);
}

widget::~widget() {
	instances.erase(this);
}

void widget::addToViewport(bool override) {
	if (override) {
		if (currWidget && currWidget != this)
			currWidget->removeFromViewport();
		
		addedToViewport();

		currWidget = this;
		
		GetCharacter()->setCanMove(false);
	}

	visible = true;
}

void widget::removeFromViewport() {
	if (currWidget == this) {
		// keep widget in memory until change the world
		// then once change the world load all the widgets of that world back into memory during the loading
		//delete Main::currWidget; // removes widget from memory
		currWidget = nullptr;
	}

	GetCharacter()->setCanMove(true);

	visible = false;
}

void widget::draw(Shader* shaderProgram) {

}

bool widget::isVisible() {
	return visible;
}

void widget::setVisibility(bool visible) {
	this->visible = visible;
}

bool widget::mouseOver() {
	vector mousePos = Input::getMousePos();
	vector loc = getLoc();
	vector size = getSize();
	if (mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		return true;
	return false;
}

void widget::resizeScreen() {
	for (widget* wig : instances)
		wig->setupLocs();
}

void widget::setupLocs() {

}

vector widget::getLoc() {
	if (parent)
		return loc;
	return loc;
}

vector widget::getSize() {
	return size;
}

void widget::setLoc(vector loc) {
	this->loc = loc;
}

void widget::setOgLoc(vector loc) {
	ogLoc = loc;
}

void widget::setSize(vector size) {
	this->size = size;

}
void widget::setLocAndSize(vector loc, vector size) {
	setLoc(loc);
	setSize(size);
}

void widget::addedToViewport() {

}

widget* widget::getCurrWidget() {
	return currWidget;
}

widget* widget::getParent() {
	return parent;
}

void widget::setParent(widget* newParent) {
	// its the same parent
	if (parent == newParent)
		return;

	if (this == newParent) {
		std::cerr << "Parent cannot be itself\n";
		return;
	}

	// when removing object from parent, remove this child from parents child list, then update this and all childrens root parents
	if (parent) {
		// remove this from parents child list
		std::vector<widget*>& siblings = this->parent->children;
		auto it = std::find(siblings.begin(), siblings.end(), this);
		if (it != siblings.end())
			siblings.erase(it);
	}

	// update parent
	parent = newParent;
	
	// update root parent
	rootParent = recalcRootParent();

	// add this to new parents child list
	if (parent)
		parent->children.push_back(this);

	// update recursively all childrens parents to new root parent
	updateAllChildrensRootParent(this, rootParent);
}

widget* widget::recalcRootParent() {
	widget* curr = this;
	while (curr->getParent() != nullptr)
		curr = curr->getParent();
	return curr;
}

void widget::updateAllChildrensRootParent(widget* curr, widget* newRootParent) {
	// DFS all children and set new root parent
	if (!curr) return;
	for (widget* child : curr->children) {
		child->setRootParent(newRootParent);
		updateAllChildrensRootParent(child, newRootParent);
	}
}

widget* widget::getRootParent() {
	return rootParent;
}

void widget::setRootParent(widget* rootParent) {
	this->rootParent = rootParent;
}