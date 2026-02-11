#include "button.h"
#include "Input.h"
#include "animation.h"
#include "Audio.h"
#include "background.h"
#include "character.h"

#include "debugger.h"

Ubutton::Ubutton(widget* parent, std::string spriteSheetPath, int cellWidth, int cellHeight, int numberOfFrames, vector loc, bool useWorldLoc, bool useAlpha) : widget(parent) {
	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "click", animDataStruct({ 0, 0 }, { float(numberOfFrames - 1), 0 }, false) });
	animData.insert({ "hover", animDataStruct({ 0, 1 }, { float(numberOfFrames - 1), 1 }, false) });
	animData.insert({ "disabled", animDataStruct({ 0, 2 }, { 1, 2 }, false) });
	buttonAnim = std::make_shared<animation>(spriteSheetPath, cellWidth, cellHeight, animData, useWorldLoc, loc);
	buttonAnim->setAnimation("click");

	clickAudio = std::make_unique<Audio>("click.wav", AudioType::SFX);

	hasHover = buttonAnim->GetCellNum().y >= 2;
	hasDisabled = buttonAnim->GetCellNum().y >= 3;


	this->useAlpha = useAlpha;
	buttonAnim->SetUseAlpha(useAlpha);

	this->useWorldLoc = useWorldLoc;

	setSize({ static_cast<float>(cellWidth), static_cast<float>(cellHeight) });
	setLoc(loc);
}

Ubutton::Ubutton(widget* parent, vector size) : widget(parent) {
	clickAudio = std::make_unique<Audio>("click.wav", AudioType::SFX);
	background = std::make_unique<Background>(this, "widget/background/button", glm::vec4(242.0 / 255.0, 233.0 / 255.0, 211.0 / 255.0, 1.0));
	background->setSize(size);
	this->useAlpha = false;
	this->useWorldLoc = false;
	setSize(size);
	setLoc(vector(0, 0));
}

void Ubutton::addCallback(void (*callback) ()) {
	callback_ = callback;
}

void Ubutton::draw(Shader* shaderProgram) {
	onHover(shaderProgram);
}

void Ubutton::onHover(Shader* shaderProgram) {
	if (buttonAnim)
		buttonAnim->draw(shaderProgram);
	else if (background) {
		background->setSize(size);
		background->draw(shaderProgram);
	}

	prevMouseOver = mouseOver;

	mouseOver = isMouseOver();
	if (useWorldLoc)
		mouseOver = mouseOver && GetCharacter()->CanPlayerReach(loc);

	if (mouseOver) {
		IHoverable::setHoveredItem(this);
		if (isEnabled && !prevMouseOver && hasHover && buttonAnim)
			buttonAnim->setAnimation("hover");
		if (Input::getMouseButtonDown(MOUSE_BUTTON_LEFT))
			Input::setLeftClick(this, &Ubutton::onClick);
	} else if (!mouseOver && buttonAnim && prevMouseOver)
		buttonAnim->setAnimation("click");
}

bool Ubutton::isMouseOver() {
	if (buttonAnim)
		return buttonAnim->IsMouseOver(useAlpha);
	else if (background)
		return background->mouseOver();
}

void Ubutton::setLoc(vector loc) {
	loc = loc.floor();
	__super::setLoc(loc);
	
	if (buttonAnim)
		buttonAnim->setLoc(absoluteLoc);
	else if (background)
		background->setLoc(absoluteLoc);
}

void Ubutton::onClick() {
	if (!isEnabled)
		return;

	if (hasHover && buttonAnim) {
		buttonAnim->setAnimation("hover");
		buttonAnim->start();
	}

	clickAudio->Play();

	if (callback_)
		callback_();
}

void Ubutton::enable(bool enabled) {
	isEnabled = enabled;
	if (!buttonAnim)
		return;

	if (isEnabled) {
		IHoverable::setCursorHoverIcon(CURSOR_POINT);
		if (isMouseOver())
			buttonAnim->setAnimation("hover");
	} else {
		IHoverable::setCursorHoverIcon(CURSOR_DEFAULT);
		buttonAnim->setAnimation("click");
	}
}

vector Ubutton::getSize() {
	if (buttonAnim)
		return buttonAnim->GetCellSize();
	else if (background)
		return background->getSize();
	return vector(0, 0);
}

void Ubutton::SetColorMod(glm::vec4 colorMod) {
	if (buttonAnim)
		buttonAnim->SetColorMod(colorMod);
}

void Ubutton::SetClickAudio(std::string path) {
	clickAudio->SetAudio(path);
}