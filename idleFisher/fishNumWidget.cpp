#include "fishNumWidget.h"

#include <iostream>

#include "stuff.h"
#include "shortNumbers.h"
#include "saveData.h"
#include "image.h"
#include "text.h"

#include "debugger.h"

UfishNumWidget::UfishNumWidget(widget* parent) : widget(parent) {
	
}

// setup / update
void UfishNumWidget::setup(FfishData* fish, FsaveFishData* saveFish, int fishQuality) {
	this->fish = fish;
	this->saveFish = saveFish;
	this->fishQuality = fishQuality;

	thumbnail = std::make_unique<Image>(fish->thumbnail, vector{ 0, 0 }, false);
	
	if (fishQuality == 1)
		star = std::make_unique<Image>("images/widget/tinyBronzeStar.png", vector{ 0, 0 }, false);
	else if (fishQuality == 2)
		star = std::make_unique<Image>("images/widget/tinySilverStar.png", vector{ 0, 0 }, false);
	else if (fishQuality == 3)
		star = std::make_unique<Image>("images/widget/tinyGoldStar.png", vector{ 0, 0 }, false);

	setupText();
	setLoc(loc);
}

void UfishNumWidget::setup(FcurrencyStruct* currency, double num) {
	thumbnail = std::make_unique<Image>("images/currency/coin" + std::to_string(currency->id) + ".png", vector{0, 0}, false);

	currencyNum = num;
	setupText();
	setLoc(loc);
}

UfishNumWidget::~UfishNumWidget() {

}

void UfishNumWidget::setupText() {
	numText = std::make_unique<text>(this, shortNumbers::convert2Short(saveFish ? saveFish->numOwned[fishQuality] : currencyNum), "straight", vector{ 0.f, 0.f });
}

void UfishNumWidget::draw(Shader* shaderProgram) {
	if (thumbnail)
		thumbnail->draw(shaderProgram);
	if (numText)
		numText->draw(shaderProgram);
	if (star && fishQuality != 0)
		star->draw(shaderProgram);
}

void UfishNumWidget::setLoc(vector loc) {
	__super::setLoc(loc);

	if (thumbnail)
		thumbnail->setLoc(absoluteLoc);
	
	if (numText && thumbnail) {
		numText->setLoc((absoluteLoc + vector{ 18.f, (thumbnail->getSize().y - numText->getSize().y) / 2.f }));
		setSize(thumbnail->getSize() + vector(numText->getSize().x, 0.f));
	}

	if (fishQuality != 0 && star)
		star->setLoc((absoluteLoc + vector{ 10.f, 0.f }));
}
