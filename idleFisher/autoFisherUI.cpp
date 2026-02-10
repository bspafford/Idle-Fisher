#include "autoFisherUI.h"

#include <string>

#include "AautoFisher.h"
#include "shortNumbers.h"
#include "math.h" 

#include "AFmoreInfoUI.h"
#include "animation.h"
#include "button.h"
#include "text.h"
#include "upgrades.h"
#include "Scene.h"

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

	pressTest = std::make_unique<Image>("images/autoFisher/UI/multipliers/multi3.png", UILoc, true);

	menuButton = std::make_unique<Ubutton>(this, "autoFisher/UI/menu.png", 5, 10, 1, UILoc + vector{ 0.f, 37.f }, true, false);
	menuButton->addCallback(this, &autoFisherUI::openMenu);

	// level
	levelBar = std::make_unique<Image>("images/autoFisher/UI/level1.png", UILoc, true);

	levelText = std::make_unique<text>(this, std::to_string(SaveData::saveData.progressionData.at(autoFisher->id).level), "tall", UILoc + vector{31.f, 67.f}, true, true, TEXT_ALIGN_RIGHT);
	maxText = std::make_unique<text>(this, " ", "tall", UILoc + vector{ 31.f, 66.f }, true, true);
	maxText->setTextColor(glm::vec4(124.f / 255.f, 127.f / 255.f, 85.f / 255.f, 1.f));
	buttonTextLoc = UILoc + vector{ 34.f, 23.f };
	buttonText = std::make_unique<text>(this, shortNumbers::convert2Short(Upgrades::GetPrice(autoFisher->id)), "normal", buttonTextLoc, true, true, TEXT_ALIGN_RIGHT);

	std::string levelBarPath = "images/autoFisher/UI/level";
	for (int i = 0; i < 21; i++)
		levelBarImgs.push_back(levelBarPath + std::to_string(i + 1) + ".png");

	std::unordered_map<std::string, animDataStruct> openData;
	openData.insert({ "open", animDataStruct({0, 0}, {3, 0}, false) });
	openData.insert({ "close", animDataStruct({0, 1}, {3, 1}, false) });
	openAnimation = std::make_unique<animation>("autoFisher/UI/openUI/openUI.png", 36, 66, openData, true, UILoc);
	openAnimation->setAnimation("open");

	openAudio = std::make_unique<Audio>("temp/openUI.mp3", AudioType::SFX);
}

autoFisherUI::~autoFisherUI() {

}

void autoFisherUI::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	if (closing || opening) {
		openAnimation->draw(shaderProgram);
	} else {
		if (upgradeButton) {
			upgradeButton->draw(shaderProgram);
			// if has enough currency to upgrade it
			upgradeButton->enable(upgradeCost <= SaveData::saveData.currencyList.at(Scene::GetCurrWorldId()).numOwned);
		}
		levelBar->draw(shaderProgram);


		if (pressTest && autoFisher->upgradeAmount == 1)
			pressTest->draw(shaderProgram);
		if (multiMax && autoFisher->upgradeAmount != -1) // max
			multiMax->draw(shaderProgram);
		if (multi10 && autoFisher->upgradeAmount != 10)
			multi10->draw(shaderProgram);
		else if (pressTest && autoFisher->upgradeAmount == 10)
			pressTest->draw(shaderProgram);
		if (multi1 && autoFisher->upgradeAmount != 1)
			multi1->draw(shaderProgram);
		if (pressTest && autoFisher->upgradeAmount == -1) // max
			pressTest->draw(shaderProgram);
		
			
		if (menuButton)
			menuButton->draw(shaderProgram);

		// text
		levelText->draw(shaderProgram);
		ProgressionNode& data = SaveData::data.progressionData.at(autoFisher->id);
		int level = SaveData::saveData.progressionData.at(autoFisher->id).level;
		int maxLevel = data.maxLevel;
		if (level < maxLevel)
			maxText->draw(shaderProgram);

		if (level >= maxLevel || Upgrades::GetPrice(autoFisher->id, autoFisher->upgradeAmount) <= SaveData::saveData.currencyList.at(data.worldId).numOwned)
			buttonText->setTextColor(glm::vec4(1));
		else
			buttonText->setTextColor(glm::vec4(1, 0, 0, 1));

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

	ProgressionNode& data = SaveData::data.progressionData.at(autoFisher->id);
	SaveEntry& saveData = SaveData::saveData.progressionData.at(autoFisher->id);
	int calculatedUpgradeAmount;
	upgradeCost = Upgrades::GetPrice(data, saveData, autoFisher->upgradeAmount, &calculatedUpgradeAmount);

	int level = SaveData::saveData.progressionData.at(autoFisher->id).level;
	int maxLevel = SaveData::data.progressionData.at(autoFisher->id).maxLevel;

	// update level num
	levelText->setText(std::to_string(level));
	maxText->setText("+" + std::to_string(calculatedUpgradeAmount));

	maxText->setLoc(levelText->getLoc() - vector{ 0, maxText->getSize().y + 5.f });

	if (level < maxLevel)
		buttonText->setText(shortNumbers::convert2Short(upgradeCost));
	else {
		buttonText->setText("max");
		upgradeButton->enable(false);
	}

	// based on the level up the level bar
	int levelIndex = static_cast<int>(floor((float(level) / float(maxLevel)) * ((int)levelBarImgs.size() - 1)));
	levelBar = std::make_unique<Image>(levelBarImgs[levelIndex], UILoc, true);
}

void autoFisherUI::openMenu() {
	autoFisher->afMoreInfoUI->addToViewport(nullptr);
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

	openAudio->Play();

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
	autoFisher->upgradeAmount = multiplier;

	std::string path = "images/autoFisher/UI/multipliers/multi";

	std::string mult;
	if (autoFisher->upgradeAmount == 1) {
		mult = "1";
		pressTest->setImage(path + "3.png");
	} else if (autoFisher->upgradeAmount == 10) {
		mult = "10";
		pressTest->setImage(path + "12.png");
	} else { // max
		mult = "max";
		pressTest->setImage(path + "Max3.png");
	}

	updateUI();
}

void autoFisherUI::setMulti1() {
	setMultiplier(1);
}
void autoFisherUI::setMulti10() {
	setMultiplier(10);
}
void autoFisherUI::setMultiMax() {
	setMultiplier(-1);
}