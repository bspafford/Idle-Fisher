#include "autoFisherUI.h"

#include <string>

#include "AautoFisher.h"
#include "shortNumbers.h"
#include "math.h" 

#include "AFmoreInfoUI.h"
#include "animation.h"
#include "button.h"
#include "text.h"

#include "debugger.h"

autoFisherUI::autoFisherUI(widget* parent, AautoFisher* autoFisherRef, vector loc) : widget(parent) {
	autoFisher = autoFisherRef;

	UILoc = loc + vector{ -12, 47 };

	// button
	upgradeButton = std::make_unique<Ubutton>(this, "autoFisher/UI/button.png", 36, 66, 4, UILoc, true, true);
	upgradeButton->addCallback(this, &autoFisherUI::upgrade);
	upgradeButton->buttonAnim->addFrameCallback(this, &autoFisherUI::moveButtonText);

	// multipliers
	multi1 = std::make_unique<Ubutton>(this, "autoFisher/UI/multipliers/multi1.png", 36, 66, 3, UILoc, true, true);
	multi10 = std::make_unique<Ubutton>(this, "autoFisher/UI/multipliers/multi10.png", 36, 66, 3, UILoc, true, true);
	multiMax = std::make_unique<Ubutton>(this, "autoFisher/UI/multipliers/multiMax.png", 36, 66, 3, UILoc, true, true);
	multi1->addCallback(this, &autoFisherUI::setMulti1);
	multi10->addCallback(this, &autoFisherUI::setMulti10);
	multiMax->addCallback(this, &autoFisherUI::setMultiMax);

	pressTest = std::make_unique<Image>("./images/autoFisher/UI/multipliers/multi3.png", UILoc, true);

	menuButton = std::make_unique<Ubutton>(this, "autoFisher/UI/menu.png", 5, 10, 1, UILoc + vector{ 0, 37 }, true, false);
	menuButton->addCallback(this, &autoFisherUI::openMenu);

	// level
	level = std::make_unique<Image>("./images/autoFisher/UI/level1.png", UILoc, true);

	levelText = std::make_unique<text>(this, std::to_string(*autoFisher->level), "tall", UILoc + vector{ 31, 64 }, true, true, TEXT_ALIGN_RIGHT);
	maxText = std::make_unique<text>(this, " ", "tall", UILoc + vector{31, 66}, true, true);
	maxText->setTextColor(124, 127, 85);
	buttonTextLoc = UILoc + vector{ 34, 20 };
	buttonText = std::make_unique<text>(this, shortNumbers::convert2Short(autoFisher->getUpgradeCost()[1]), "normal", buttonTextLoc, true, true, TEXT_ALIGN_RIGHT);

	std::string levelBarPath = "./images/autoFisher/UI/level";
	for (int i = 0; i < 21; i++)
		levelBarImgs.push_back(levelBarPath + std::to_string(i + 1) + ".png");

	std::unordered_map<std::string, animDataStruct> openData;
	openData.insert({ "open", animDataStruct({0, 0}, {3, 0}, false) });
	openData.insert({ "close", animDataStruct({0, 1}, {3, 1}, false) });
	openAnimation = std::make_unique<animation>("autoFisher/UI/openUI/openUI.png", 36, 66, openData, true, UILoc);
	openAnimation->setAnimation("open");
}

autoFisherUI::~autoFisherUI() {

}

void autoFisherUI::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	if (closing || opening) {
		openAnimation->draw(shaderProgram);
	} else {
		if (upgradeButton)
			upgradeButton->draw(shaderProgram);
		level->draw(shaderProgram);


		if (pressTest && autoFisher->multiplier == 1)
			pressTest->draw(shaderProgram);
		if (multiMax && autoFisher->multiplier != int(INFINITY))
			multiMax->draw(shaderProgram);
		if (multi10 && autoFisher->multiplier != 10)
			multi10->draw(shaderProgram);
		else if (pressTest && autoFisher->multiplier == 10)
			pressTest->draw(shaderProgram);
		if (multi1 && autoFisher->multiplier != 1)
			multi1->draw(shaderProgram);
		if (pressTest && autoFisher->multiplier == int(INFINITY))
			pressTest->draw(shaderProgram);
		
			
		if (menuButton)
			menuButton->draw(shaderProgram);

		// text
		levelText->draw(shaderProgram);
		if (*autoFisher->level < autoFisher->maxLevel)
			maxText->draw(shaderProgram);
		buttonText->draw(shaderProgram);
	}
}

void autoFisherUI::upgrade() {
	if (!visible)
		return;

	autoFisher->upgrade();

	updateUI();
}

void autoFisherUI::moveButtonText(int frame) {
	std::vector<vector> delta = std::vector<vector>{ {0, 0}, { -3, 3 }, {1, -1}, {3, -2}, {-1, 0} };
	buttonText->setLoc(buttonText->getLoc() + delta[frame]);

	if (frame == 0)
		buttonText->setLoc(buttonTextLoc);
}

void autoFisherUI::updateUI() {
	if (!visible)
		return;

	std::vector<double> upgradeCost = autoFisher->getUpgradeCost();

	// if has enough currency to upgrade it
	if (upgradeButton)
		upgradeButton->enable(upgradeCost[1] <= SaveData::saveData.currencyList[autoFisher->currencyId].numOwned);


	// update level num
	levelText->setText(std::to_string(*autoFisher->level));
	maxText->setText("+" + std::to_string(int(upgradeCost[0])));

	maxText->setLoc(levelText->getLoc() - vector{0, maxText->getSize().y / stuff::pixelSize + 4});

	if (*autoFisher->level < autoFisher->maxLevel)
		buttonText->setText(shortNumbers::convert2Short(upgradeCost[1]));
	else {
		buttonText->setText("max");
		upgradeButton->enable(false);
	}

	// based on the level up the level bar
	int levelIndex = static_cast<int>(floor((float(*autoFisher->level) / float(autoFisher->maxLevel)) * ((int)levelBarImgs.size() - 1)));
	level = std::make_unique<Image>(levelBarImgs[levelIndex], UILoc, true);
}

void autoFisherUI::openMenu() {
	autoFisher->afMoreInfoUI->addToViewport(true);
}

void autoFisherUI::openUI() {
	if (closing || opening)
		return;

	if (visible) {
		closeUI();
		return;
	}

	visible = true;
	updateUI();

	// do open animation
	closing = false;
	openAnimation->stop();
	openAnimation->setAnimation("open");
	openAnimation->addFinishedCallback(this, &autoFisherUI::openedUI);
	openAnimation->start();
	opening = true;
}

void autoFisherUI::openedUI() {
	opening = false;
}

void autoFisherUI::closeUI() {
	opening = false;
	openAnimation->stop();
	openAnimation->setAnimation("close");
	openAnimation->addFinishedCallback(this, &autoFisherUI::closedUI);
	openAnimation->start();
	closing = true;
}

void autoFisherUI::closedUI() {
	visible = false;
	closing = false;
}

void autoFisherUI::setMultiplier(int multiplier) {
	autoFisher->multiplier = multiplier;

	std::string path = "./images/autoFisher/UI/multipliers/multi";

	std::string mult = "max";
	if (autoFisher->multiplier == 1) {
		mult = "1";
		pressTest->setImage(path + "3.png");
	} else if (autoFisher->multiplier == 10) {
		mult = "10";
		pressTest->setImage(path + "12.png");
	} else
		pressTest->setImage(path + "Max3.png");


	updateUI();
}

void autoFisherUI::setMulti1() {
	setMultiplier(1);
	multi1->buttonAnim->setAnimation("click");
}
void autoFisherUI::setMulti10() {
	setMultiplier(10);
}
void autoFisherUI::setMultiMax() {
	setMultiplier(int(INFINITY));
}