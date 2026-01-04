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
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	fisherNum = std::make_unique<text>(this, "#" + std::to_string(autoFisher->autoFisherNum + 1), "afScreen", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);

	levelText = std::make_unique<text>(this, "123", "afScreen", vector{ 100, 0 }, false, false, TEXT_ALIGN_RIGHT);

	fullnessText = std::make_unique<text>(this, "42/122", "afScreen", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	closeButton = std::make_unique<Ubutton>(this, "autoFisher/moreUI/xButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	closeButton->addCallback(static_cast<widget*>(this), &widget::removeFromViewport);

	fpsText = std::make_unique<text>(this, shortNumbers::convert2Short(autoFisher->calcFPS()), "afScreen", vector{ 0, 0 });

	mpsText = std::make_unique<text>(this, "123mps", "afScreen", vector{0, 0});

	levelBar = std::make_unique<Image>("./images/autoFisher/moreUI/level/level1.png", vector{ 0, 0 }, false);
	fullnessBar = std::make_unique<Image>("./images/autoFisher/moreUI/fullness/fullness1.png", vector{ 0, 0 }, false);

	setupLocs();
}

AFmoreInfoUI::~AFmoreInfoUI() {

}

void AFmoreInfoUI::setupLocs() {
	fisherNum->setLoc(background->getAbsoluteLoc() + vector{ 141.f, 178.f });
	levelText->setLoc(background->getAbsoluteLoc() + vector{ 141.f, 145.f });
	fullnessText->setLoc(background->getAbsoluteLoc() + vector{ background->getSize().x / 2.f, 111.f });
	if (closeButton)
		closeButton->setLoc({ 72.f, 98.f });
	if (fpsText)
		fpsText->setLoc(background->getAbsoluteLoc() + vector{ 34.f, 70.f});
	if (mpsText)
		mpsText->setLoc(background->getAbsoluteLoc() + vector{ 34.f, 50.f });
	fullnessBar->setLoc(background->getAbsoluteLoc());
	levelBar->setLoc(background->getAbsoluteLoc());
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
	int fullnessPercent = int(math::clamp(roundf(fullness / maxFullness * 100.f), 0.f, 100.f));
	fullnessText->setText(shortNumbers::convert2Short(fullness) + "/" + shortNumbers::convert2Short(maxFullness));
	fullnessBar->setImage("./images/autoFisher/moreUI/fullness/fullness" + std::to_string(fullnessPercent + 1) + ".png");
}

void AFmoreInfoUI::addToViewport(widget* parent) {
	__super::addToViewport(parent);
	updateUI();
}