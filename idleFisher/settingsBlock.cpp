#include "settingsBlock.h"
#include "text.h"
#include "button.h"

#include "debugger.h"

UsettingsBlock::UsettingsBlock(widget* parent, std::string optionsName, int length, std::vector<std::string> options, bool* settingsValue) : widget(parent) {
	this->options = options;
	this->settingsValue = settingsValue;
	currOptionsIdx = *this->settingsValue;

	setSize({ float(length), 20 * stuff::pixelSize });

	optionsTitle = std::make_unique<text>(this, optionsName, "straight", vector{ 0, 0 });
	optionsButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	optionsButton->addCallback(this, &UsettingsBlock::toggleOption);
	selectedOptionText = std::make_unique<text>(this, options[currOptionsIdx], "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
}

void UsettingsBlock::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	optionsTitle->draw(shaderProgram);
	optionsButton->draw(shaderProgram);
	selectedOptionText->draw(shaderProgram);
}

void UsettingsBlock::addCallback(void (*callback) ()) {
	callback_ = callback;
}

void UsettingsBlock::setLoc(vector loc) {
	__super::setLoc(loc);

	optionsTitle->setLoc({loc.x + 10.f * stuff::pixelSize, loc.y + size.y / 2.f });
	optionsButton->setLoc({loc.x + size.x - optionsButton->getSize().x, loc.y + size.y / 2.f });
	selectedOptionText->setLoc(optionsButton->getLoc() + optionsButton->getSize() / 2.f);
}

void UsettingsBlock::toggleOption() {
	currOptionsIdx++;
	if (currOptionsIdx >= options.size())
		currOptionsIdx = 0;

	SetValue(currOptionsIdx);
	
	if (callback_)
		callback_();
}

std::string UsettingsBlock::getSelectedOption() {
	return options[currOptionsIdx];
}

void UsettingsBlock::UpdateValue() {
	SetValue(*settingsValue);
}

void UsettingsBlock::SetValue(int value) {
	selectedOptionText->setText(options[value]);
	*settingsValue = value;
	currOptionsIdx = value;
}