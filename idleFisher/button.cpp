#include "button.h"
#include "Input.h"
#include "sounds.h"
#include "animation.h"

#include "debugger.h"

// only give NON NULL values to overriding widgets ONLY
Ubutton::Ubutton(widget* parent, std::string spriteSheetPath, int cellWidth, int cellHeight, int numberOfFrames, vector loc, bool useWorldLoc, bool useAlpha) : widget(parent) {
	// buttons will have a sprite sheet with click anim on top, and hover right below it, and prolly also a disabled one too
	// needs:
	// sprite sheet path, cell size, number of frames

	widgetClass = parent;
	this->loc = loc;

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "click", animDataStruct({ 0, 0 }, { float(numberOfFrames - 1), 0 }, false) });
	animData.insert({ "hover", animDataStruct({ 0, 1 }, { float(numberOfFrames - 1), 1 }, false) });
	animData.insert({ "disabled", animDataStruct({ 0, 2 }, { 1, 2 }, false) });
	buttonAnim = std::make_shared<animation>(spriteSheetPath, cellWidth, cellHeight, animData, useWorldLoc, loc);
	buttonAnim->setAnimation("click");

	hasHover = buttonAnim->GetCellNum().y >= 2;
	hasDisabled = buttonAnim->GetCellNum().y >= 3;


	this->useAlpha = useAlpha;
	buttonAnim->spriteSheet->setUseAlpha(useAlpha);

	this->useWorldLoc = useWorldLoc;
}

Ubutton::~Ubutton() {

}

void Ubutton::addCallback(void (*callback) ()) {
	callback_ = callback;
}

void Ubutton::draw(Shader* shaderProgra) {
	onHover(shaderProgra);
}

void Ubutton::onHover(Shader* shaderProgra) {
	if (buttonAnim)
		buttonAnim->draw(shaderProgra);

	prevMouseOver = mouseOver;
	mouseOver = isMouseOver();

	if (mouseOver && isEnabled) {
		IHoverable::setHoveredItem(this);
		if (!prevMouseOver && hasHover && buttonAnim)
			buttonAnim->setAnimation("hover");
		if (Input::getMouseButtonDown(MOUSE_BUTTON_LEFT))
			Input::setLeftClick(this, &Ubutton::onClick);
	} else if (!mouseOver && buttonAnim && prevMouseOver)
		buttonAnim->setAnimation("click");
}

bool Ubutton::isMouseOver() {
	if (!buttonAnim)
		return false;
	return buttonAnim->spriteSheet->isMouseOver(useAlpha);
}

void Ubutton::setLoc(vector loc) {
	loc = loc.floor();
	__super::setLoc(loc);
	
	buttonAnim->setLoc(loc);
}

void Ubutton::onClick() {
	if (!isEnabled)
		return;

	if (hasHover && buttonAnim) {
		buttonAnim->setAnimation("hover");
		buttonAnim->start();
	}

	// Mix_PlayChannel(-1, sounds::buttonClick, 0);

	if (callback_)
		callback_();
}

void Ubutton::enable(bool enabled) {
	isEnabled = enabled;

	// sets it to normal animation if the button is hovered
	if (hasHover && !isEnabled) {
		buttonAnim->setAnimation("hover");
	}
}

std::weak_ptr<Image> Ubutton::getButtonImg() {
	if (buttonAnim)
		return buttonAnim->spriteSheet;
	return std::weak_ptr<Image>();
}

vector Ubutton::getSize() {
	return buttonAnim->GetCellSize();
}

vector Ubutton::getLoc() {
	return loc;
}