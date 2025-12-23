#include "AFmoreInfoUI.h"

#include <iostream>

#include "main.h"
#include "shortNumbers.h"
#include "AautoFisher.h"
#include "math.h"

// widgets
#include "text.h"
#include "button.h"

#include "debugger.h"

AFmoreInfoUI::AFmoreInfoUI(widget* parent, AautoFisher* autoFisher) : widget(parent) {
	this->autoFisher = autoFisher;

	background = std::make_unique<Image>("./images/autoFisher/moreUI/UI.png", vector{ 0, 0 }, false);

	fisherNum = std::make_unique<text>(this, "#" + std::to_string(autoFisher->autoFisherNum + 1), "afScreen", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);

	levelText = std::make_unique<text>(this, "123", "afScreen", vector{ 100, 0 }, false, false, TEXT_ALIGN_RIGHT);

	fullnessText = std::make_unique<text>(this, "42/122", "afScreen", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	closeButton = std::make_unique<Ubutton>(this, "autoFisher/moreUI/xButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->addCallback(this, &AFmoreInfoUI::closeUI);

	fpsText = std::make_unique<text>(this, shortNumbers::convert2Short(autoFisher->calcFPS()), "afScreen", vector{ 0, 0 });

	mpsText = std::make_unique<text>(this, "123mps", "afScreen", vector{0, 0});

	levelBar = std::make_unique<Image>("./images/autoFisher/moreUI/level/level1.png", vector{ 0, 0 }, false);
	fullnessBar = std::make_unique<Image>("./images/autoFisher/moreUI/fullness/fullness1.png", vector{ 0, 0 }, false);

	setupLocs();
}

AFmoreInfoUI::~AFmoreInfoUI() {

}

void AFmoreInfoUI::setupLocs() {
	background->setLoc((stuff::screenSize - background->getSize()) / 2.f);
	fisherNum->setLoc({ stuff::screenSize.x / 2.f + 54 * stuff::pixelSize,  stuff::screenSize.y / 2.f - 73 * stuff::pixelSize });
	levelText->setLoc({stuff::screenSize.x / 2.f + 54 * stuff::pixelSize,  stuff::screenSize.y / 2.f - 40 * stuff::pixelSize});
	fullnessText->setLoc({ stuff::screenSize.x / 2.f,  stuff::screenSize.y / 2.f - 6 * stuff::pixelSize });
	if (closeButton)
		closeButton->setLoc({ stuff::screenSize.x / 2.f + 72 * stuff::pixelSize,  stuff::screenSize.y / 2.f - 108 * stuff::pixelSize });
	if (fpsText)
		fpsText->setLoc({ stuff::screenSize.x / 2.f - 54 * stuff::pixelSize,  stuff::screenSize.y / 2.f + 28 * stuff::pixelSize });
	if (mpsText)
		mpsText->setLoc({ stuff::screenSize.x / 2.f - 54 * stuff::pixelSize,  stuff::screenSize.y / 2.f + 48 * stuff::pixelSize });
	fullnessBar->setLoc(background->getLoc());
	levelBar->setLoc(background->getLoc());
}

void AFmoreInfoUI::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	background->draw(shaderProgram);

	fisherNum->draw(shaderProgram);
	levelText->draw(shaderProgram);
	fullnessText->draw(shaderProgram);
	fpsText->draw(shaderProgram);
	mpsText->draw(shaderProgram);
	fullnessBar->draw(shaderProgram);
	levelBar->draw(shaderProgram);

	closeButton->draw(shaderProgram);
}

void AFmoreInfoUI::updateUI() {
	fpsText->setText(shortNumbers::convert2Short(autoFisher->calcFPS(), true) + "fps");
	mpsText->setText(shortNumbers::convert2Short(autoFisher->calcMPS(), true) + "mps");

	double level = *autoFisher->level;
	double maxLevel = autoFisher->maxLevel;
	int levelPercent = int(round(level / maxLevel * 100));
	levelText->setText(std::to_string(*autoFisher->level));
	levelBar->setImage("./images/autoFisher/moreUI/level/level" + std::to_string(levelPercent + 1) + ".png");

	double fullness = autoFisher->calcCurrencyHeld();
	double maxFullness = autoFisher->maxCurrency;
	int fullnessPercent = int(round(fullness / maxFullness * 100));
	fullnessText->setText(shortNumbers::convert2Short(fullness) + "/" + shortNumbers::convert2Short(maxFullness));
	fullnessBar->setImage("./images/autoFisher/moreUI/fullness/fullness" + std::to_string(fullnessPercent + 1) + ".png");
}

// kinda like a call back function
void AFmoreInfoUI::addToViewport(bool override) {
	__super::addToViewport(override);

	updateUI();
}

void AFmoreInfoUI::closeUI() {
	// because when the button disapears it doesn't remove hover num
	removeFromViewport();
}