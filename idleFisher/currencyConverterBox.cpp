#include "currencyConverterBox.h"

#include "Input.h"
#include "character.h"
#include "saveData.h"
#include "upgrades.h"
#include "shortNumbers.h"
#include "animation.h"
#include "text.h"
#include "button.h"
#include "verticalBox.h"
#include "currencyConverterWidget.h"
#include "atm.h"

#include "debugger.h"

UcurrencyConverterBox::UcurrencyConverterBox(widget* parent, FcurrencyStruct* currencyStruct, FsaveCurrencyStruct* saveCurrencyStruct) : widget(parent) {
	this->currencyStruct = currencyStruct;
	this->saveCurrencyStruct = saveCurrencyStruct;

	nameString = currencyStruct->name;
	descriptionString = SaveData::data.currencyData.at(currencyStruct->id).name;

	setup();
}

UcurrencyConverterBox::~UcurrencyConverterBox() {

}

void UcurrencyConverterBox::setup() {
	background = std::make_unique<Image>("images/widget/upgradeBoxBackground.png", vector{ 0, 0 }, false);

	//thumbnailBackground = std::make_unique<Image("images/widget/thumbnailBackground.png");
	//name = std::make_unique<text(nameString, "straight", { 0, 0 });

	std::string currencyThumbnail = "images/currency/coin" + std::to_string(currencyStruct->id) + ".png";
	currency1 = std::make_unique<Image>(currencyThumbnail, vector{ 0, 0 }, false);
	currency1Num = std::make_unique<text>(this, " ", "straight", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);
	currency2 = std::make_unique<Image>(currencyThumbnail, vector{0, 0}, false);
	currency2Num = std::make_unique<text>(this, " ", "straight", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);
	arrow = std::make_unique<Image>("images/widget/arrow.png", vector{ 0, 0 }, false);

	buyButton = std::make_unique<Ubutton>(widgetParent, "images/widget/upgradeButton.png", 35, 13, 2, vector{ 0, 0 }, false, false);
	buyButton->addCallback(this, &UcurrencyConverterBox::buyUpgrade);

	buttonPriceText = std::make_unique<text>(this, "Start", "straightDark", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);
	buttonPriceText->setTextColor(glm::vec4(1, 0, 0, 1));

	// if going
	FcurrencyConversionStruct* currencyConversionStruct = &SaveData::saveData.currencyConversionList.at(currencyStruct->id);
	if (currencyConversionStruct->converting) {
		Aatm* atm = dynamic_cast<Aatm*>(widgetParent->parent);
		if (atm) {
			//atm->startConversion(currencyStruct, saveCurrencyStruct, currencyConversionStruct);
			buttonPriceText->setText("Stop");
			setupLocs(); // updates the locks of the button and the upgrade text
		}
	}

	//update();

	update();
}

void UcurrencyConverterBox::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	background->draw(shaderProgram);

	//thumbnailBackground->draw(shaderProgram, false);
	currency1->draw(shaderProgram);
	currency1Num->draw(shaderProgram);
	currency2->draw(shaderProgram);
	currency2Num->draw(shaderProgram);
	arrow->draw(shaderProgram);
	buyButton->draw(shaderProgram);

	buttonPriceText->draw(shaderProgram);

	if (upgradeText)
		upgradeText->draw(shaderProgram);
	// changes the text only if it isn't already set to current world
	if (mouseOver()) {
		if (widgetParent && widgetParent->name->getString() != nameString)
			widgetParent->setNameDescription(nameString, descriptionString);
	}
}

void UcurrencyConverterBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc, size);
	setupLocs();
}

void UcurrencyConverterBox::setupLocs() {
	background->setLoc(loc);

	vector buttonSize = buyButton->getSize();
	vector buyButtonLoc = loc + vector{ size.x - buttonSize.x, size.y / 2 - buttonSize.y / 2 } + vector{ -10 * stuff::pixelSize, 0 };
	buyButton->setLoc(buyButtonLoc);

	buttonPriceText->setLoc(buyButtonLoc + buyButton->getSize() / 2 - vector{ 0, buttonPriceText->getSize().y / 2 });

	currency1->setLoc(loc + vector{ 6 * stuff::pixelSize, size.y / 2 - currency1->getSize().y / 2 });
	currency1Num->setLoc(currency1->getLoc() + vector{currency1->getSize().x / 2, currency1->getSize().y});
	currency2->setLoc(currency1->getLoc() + vector{currency1->getSize().x + 10 * stuff::pixelSize, 0});
	currency2Num->setLoc(currency2->getLoc() + vector{currency2->getSize().x / 2, currency2->getSize().y});
	arrow->setLoc(currency1->getLoc() + vector{currency1->getSize().x + stuff::pixelSize, currency1->getSize().y / 2 - arrow->getSize().y / 2});

	if (upgradeText) {
		vector upgradeTextLoc = { buyButtonLoc.x - stuff::pixelSize, loc.y + size.y / 2 - upgradeText->getSize().y / 2};
		upgradeText->setLoc(upgradeTextLoc);
	}
}

bool UcurrencyConverterBox::mouseOver() {
	vector mousePos = Input::getMousePos();

	if (!getParent() && mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		return true;
	else if (getParent()) {
		vector parentLoc = getParent()->getLoc();
		vector parentSize = getParent()->getSize();
		if (mousePos.x >= loc.x + parentLoc.x && mousePos.x <= loc.x + size.x + parentLoc.x && mousePos.y >= loc.y + parentLoc.y && mousePos.y <= loc.y + size.y + parentLoc.y)
			// test if inside the rect too
			if (mousePos.x >= parentLoc.x && mousePos.x <= parentLoc.x + parentSize.x && mousePos.y >= parentLoc.y && mousePos.y <= parentLoc.y + parentSize.y)
				return true;
	}
	return false;
}

void UcurrencyConverterBox::buyUpgrade() {
	Aatm* atm = dynamic_cast<Aatm*>(widgetParent->parent);
	if (!atm)
		return;

	FcurrencyConversionStruct* currencyConversionStruct = &SaveData::saveData.currencyConversionList.at(currencyStruct->id);
	if (currencyConversionStruct->converting) {
		//atm->stopConversion(currencyStruct, saveCurrencyStruct, currencyConversionStruct);
		buttonPriceText->setText("Start");
		// start stuff in atm npc
	} else {
		//atm->startConversion(currencyStruct, saveCurrencyStruct, currencyConversionStruct);
		buttonPriceText->setText("Stop");
	}

	setupLocs(); // updates the locks of the button and the upgrade text
}

void UcurrencyConverterBox::update() {
	// if going
	/*
	if () { // if max
		buyButton->setImg("images/widget/upgradeButton3.png");
		buttonPriceText->setText("Stop");
	} */

	FcurrencyConversionStruct conversion = SaveData::saveData.currencyConversionList.at(currencyStruct->id);
	currency1Num->setText(shortNumbers::convert2Short(conversion.price));
	currency2Num->setText(shortNumbers::convert2Short(conversion.yield));
	setupLocs();
}

vector UcurrencyConverterBox::getSize() {
	return background->getSize();
}