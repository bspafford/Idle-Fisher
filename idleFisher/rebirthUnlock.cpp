#include "rebirthUnlock.h"
#include "saveData.h"
#include "shortNumbers.h"
#include "text.h"
#include "button.h"
#include "rebirthWidget.h"

#include "debugger.h"

UrebirthUnlock::UrebirthUnlock(widget* parent, int id) : widget(parent) {
	rebirthInfo = &SaveData::data.rebirthData[id];
	saveRebirthInfo = &SaveData::saveData.rebirthList[id];

	background = std::make_unique<Image>("./images/widget/rebirthIcons/background.png", vector{ 0, 0 }, false);

	// removes "./images/" from the string
	std::string thumbnailPath = rebirthInfo->thumbnailPath;
	thumbnailPath.erase(0, 9);

	button = std::make_unique<Ubutton>(this, thumbnailPath, 16, 16, 1, vector{ 0, 0 }, false, false);
	button->addCallback(this, &UrebirthUnlock::onClick);

	upgradeCost = std::make_unique<text>(this, shortNumbers::convert2Short(rebirthInfo->currencyNum), "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	if (!saveRebirthInfo->unlocked) {
		if (background)
			background->setColorMod(glm::vec4(75.f/255.f, 75.f/255.f, 75.f/255.f, 1.f));
		if (button && button->getButtonImg().lock())
			button->getButtonImg().lock()->setColorMod(glm::vec4(75.f / 255.f, 75.f / 255.f, 75.f / 255.f, 1.f));
	}

	if (saveRebirthInfo->unlocked)
		unlock();
}

UrebirthUnlock::~UrebirthUnlock() {

}

void UrebirthUnlock::draw(Shader* shaderProgram) {
	background->draw(shaderProgram);
	button->draw(shaderProgram);
}

void UrebirthUnlock::onClick() {
	if (!saveRebirthInfo->unlocked && prerequisitesMet() && SaveData::saveData.rebirthCurrency >= rebirthInfo->currencyNum) {
		SaveData::saveData.rebirthCurrency -= rebirthInfo->currencyNum;
		static_cast<UrebirthWidget*>(getParent())->update();
		unlock();
	}
}

bool UrebirthUnlock::prerequisitesMet() {
	for (int id : rebirthInfo->prerequisites)
		if (!SaveData::saveData.rebirthList[id].unlocked)
			return false;
	return true;
}

void UrebirthUnlock::unlock() {
	saveRebirthInfo->unlocked = true;
	upgradeCost->setVisibility(false);

	background->setColorMod(glm::vec4(1.f));
	button->getButtonImg().lock()->setColorMod(glm::vec4(1.f));
}

bool UrebirthUnlock::isMouseOver() {
	return button->isMouseOver();
}

void UrebirthUnlock::setLoc(vector loc) {
	__super::setLoc(loc);

	if (button) {
		button->setLoc(loc);
		background->setLoc(button->loc + button->getSize() / 2 - background->getSize() / 2);
		upgradeCost->setLoc(loc + vector{ button->getSize().x / 2, button->getSize().y + stuff::pixelSize });
	}
}

vector UrebirthUnlock::getSize() {
	return button->getSize();
}