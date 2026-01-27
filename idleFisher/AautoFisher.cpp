#include "AautoFisher.h"

#include <iostream>

#include "main.h"
#include "Input.h"
#include "worlds.h"
#include "stuff.h"
#include "animation.h"
#include "shortNumbers.h"
#include "collision.h"
#include "character.h"
#include "autoFisherUI.h"
#include "upgrades.h"

// widget
#include "heldFishWidget.h"
#include "currencyWidget.h"
#include "AFmoreInfoUI.h"
#include "autoFisherUI.h"
#include "button.h"
#include "text.h"

#include "textureManager.h"

#include "debugger.h"

AautoFisher::AautoFisher(uint32_t id) {
	if (!autoFisherSpriteSheet)
		autoFisherSpriteSheet = std::make_shared<Image>("images/autoFisher/autoFisher.png", vector{ 0, 0 }, true);
	if (!fishingLineSpriteSheet)
		fishingLineSpriteSheet = std::make_shared<Image>("images/autoFisher/fishingLine.png", vector{ 0, 0 }, true);
	if (!outlineSpriteSheet)
		outlineSpriteSheet = std::make_shared<Image>("images/autoFisher/outline.png", vector{ 0, 0 }, true);

	this->id = id;
	auto& afData = SaveData::saveData.autoFisherList.at(id);
	afData.first.level = true;
	loc = { SaveData::data.autoFisherData[id].xLoc, SaveData::data.autoFisherData[id].yLoc };
	level = &afData.first.level;

	setupCollision();
	
	upgradeAnimIndex = *level / 10;

	// auto fisher animation
	float duration = 0.1f;
	std::unordered_map<std::string, animDataStruct> autoFisherAnimData;
	autoFisherAnimData.insert({ "1", animDataStruct({0, 0}, {39, 0}, true) });
	autoFisherAnimData.insert({ "2", animDataStruct({0, 1}, {39, 1}, true) });
	autoFisherAnimData.insert({ "3", animDataStruct({0, 2}, {39, 2}, true) });
	autoFisherAnimData.insert({ "4", animDataStruct({0, 3}, {39, 3}, true) });
	autoFisherAnimData.insert({ "5", animDataStruct({0, 4}, {39, 4}, true) });
	autoFisherAnimData.insert({ "6", animDataStruct({0, 5}, {39, 5}, true) });
	autoFisherAnimData.insert({ "7", animDataStruct({0, 6}, {39, 6}, true) });
	autoFisherAnimData.insert({ "8", animDataStruct({0, 7}, {39, 7}, true) });
	autoFisherAnimData.insert({ "9", animDataStruct({0, 8}, {39, 8}, true) });
	autoFisherAnimData.insert({ "10", animDataStruct({0, 9}, {39, 9}, true) });
	autoFisherAnimData.insert({ "11", animDataStruct({0, 10}, {39, 10}, true) });

	anim = std::make_unique<animation>(autoFisherSpriteSheet, 42, 58, autoFisherAnimData, true, loc);
	anim->addFrameCallback(this, &AautoFisher::OutlineUpdate);
	anim->addAnimEvent(40, this, &AautoFisher::catchFish);
	anim->setAnimation("1");
	anim->start();

	// outline animation
	std::unordered_map<std::string, animDataStruct> outlineData;
	outlineData.insert({ "outline", animDataStruct({0, 0}, {39, 0}, true) });
	outline = std::make_unique<animation>(outlineSpriteSheet, 42, 58, outlineData, true, loc);
	outline->setAnimation("outline");

	// fishing rod animation
	std::unordered_map<std::string, animDataStruct> fishingLineData;
	fishingLineData.insert({ "fishingLine", animDataStruct({0, 0}, {39, 0}, true) });
	fishingLine = std::make_unique<animation>(fishingLineSpriteSheet, 138, 139, fishingLineData, true, loc + vector{ 13, -74 });
	fishingLine->setAnimation("fishingLine");
	fishingLine->start();

	setStats();

	autoFisherNum = world::currWorld->autoFisherList.size();

	UI = std::make_unique<autoFisherUI>(nullptr, this, loc);

	afMoreInfoUI = std::make_unique<AFmoreInfoUI>(nullptr, this);
}

AautoFisher::~AautoFisher() {
	collision::removeCollisionObject(col.get());
}

void AautoFisher::Update(float deltaTime) {
	calcIfPlayerInfront();

	bMouseOver = anim->IsMouseOver(true);
	if (anim && bMouseOver)
		IHoverable::setHoveredItem(this);

	if (Input::getMouseButtonDown(MOUSE_BUTTON_LEFT))
		leftClick();

	if (Input::getMouseButtonDown(MOUSE_BUTTON_RIGHT))
		rightClick();
}

void AautoFisher::draw(Shader* shaderProgram) {
	if (UI && UI->visible)
		UI->draw(shaderProgram);

	if (anim)
		anim->draw(shaderProgram);
	if (fishingLine)
		fishingLine->draw(shaderProgram);
	if (bMouseOver && outline)
		outline->draw(shaderProgram);
}

void AautoFisher::leftClick() {
	if (bMouseOver) {
		collectFish();
		if (UI)
			UI->updateUI();
	}
}

void AautoFisher::rightClick() {
	// show auto fisher ui
	if (bMouseOver) {
		if (UI && !UI->visible)
			// close all other autofisher UIs
			for (int i = 0; i < world::currWorld->autoFisherList.size(); i++) {
				if (world::currWorld->autoFisherList[i].get() != this && world::currWorld->autoFisherList[i]->UI->visible && !world::currWorld->autoFisherList[i]->UI->closing)
					world::currWorld->autoFisherList[i]->UI->closeUI();
				else if (!world::currWorld->autoFisherList[i]->UI->closing)
					world::currWorld->autoFisherList[i]->UI->visible = false;
			}
		if (UI)
			UI->openUI();
	}
}

// calcs if mouse over and updates the mouse over num in main
// also updates the objects bool
// only updates is theres a change in the variables
void AautoFisher::calcMouseOver(bool& mouseOverPrev, bool mouseOver) {
}

void AautoFisher::collectFish() {
	double currency = calcCurrencyHeld();
	for (int i = 0; i < heldFish.size(); i++) {
		SaveData::saveData.fishData[heldFish[i].id].numOwned[0] += heldFish[i].numOwned[0];
		SaveData::saveData.fishData[heldFish[i].id].totalNumOwned[0] += heldFish[i].numOwned[0];
		heldFish[i].numOwned[0] = 0;
	}

	Main::heldFishWidget->updateList();

	// only start if full
	if (anim && anim->IsStopped())
		anim->start();

	if (afMoreInfoUI && afMoreInfoUI->isVisible())
		afMoreInfoUI->updateUI();
}

// go through all fish and make sure the auto fisher can catch them
// if cant then remove them from the probability list
void AautoFisher::catchFish() {
	FfishData currFish = calcFish();

	int catchNum = 1;

	// if there is enough room for the fish
	if (calcCurrencyHeld() + upgrades::getFishSellPrice(currFish, 0) * catchNum <= maxCurrency) {
		int index = calcCurrencyInList(currFish, heldFish);

		if (index != -1) {
			heldFish[index].id = currFish.id;
			heldFish[index].numOwned[0]++;
		} else {
			FsaveFishData saveFish;
			saveFish.id = currFish.id;
			saveFish.numOwned[0] = catchNum;
			heldFish.push_back(saveFish);
		}

		if (anim && calcCurrencyHeld() >= maxCurrency) {
			anim->stop();
		}
	} else if (anim) {
		anim->stop();
	}

	if (afMoreInfoUI && afMoreInfoUI->isVisible())
		afMoreInfoUI->updateUI();
}

// returns -1 if currency isn't in list
int AautoFisher::calcCurrencyInList(FfishData fish, std::vector<FsaveFishData> heldFish) {
	for (int i = 0; i < heldFish.size(); i++) {
		if (heldFish[i].id == fish.id)
			return i;
	}

	return -1;
}

FfishData AautoFisher::calcFish() {
	float rand = math::randRange(0.f, 1.f);
	std::vector<std::pair<uint32_t, float>> probList = calcFishProbability(SaveData::data.fishData);

	for (int i = 0; i < probList.size(); i++) {
		if (rand <= probList[i].second)
			return SaveData::data.fishData.at(probList[i].first);
	}


	// shouldn't hit this
	return SaveData::data.fishData.begin()->second;
}

std::vector<std::pair<uint32_t, float>> AautoFisher::calcFishProbability(const std::unordered_map<uint32_t, FfishData>& fishData, bool isCurrencyAFactor) {

	float totalProb = 0;// premiumChance;

	double heldCurrency = calcCurrencyHeld();

	// starts at 1 to skip premium
	for (auto& [key, value] : fishData) {
		if (key == 1u) // premium fish id
			continue; // dont include premium here

		// see if autofisher has enough fishing power, see if theres enough room for the fish
		if (value.fishingPower <= fishingPower && (value.worldId == Scene::GetCurrWorldId() || value.worldId == 1u)) {
			if ((isCurrencyAFactor && heldCurrency + upgrades::getFishSellPrice(value, 0) <= maxCurrency) || !isCurrencyAFactor)
				totalProb += float(value.probability);
		}
	}
	std::vector<std::pair<uint32_t, float>> probList;
	float test = 0;

	// starts at 1 to skip premium
	for (auto& [key, value] : fishData) {
		if (key == 1u)
			continue; // dont include premium here

		if (value.fishingPower <= fishingPower && (value.worldId == Scene::GetCurrWorldId() || value.worldId == 1u)) {
			if ((isCurrencyAFactor && heldCurrency + upgrades::getFishSellPrice(value, 0) <= maxCurrency) || !isCurrencyAFactor) {
				test += value.probability / totalProb;
					probList.push_back(std::pair{value.id, test});
			}
		}
	}

	return probList;
}

void AautoFisher::setupCollision() {
	std::vector<vector> points = std::vector<vector>(4);
	for (int i = 0; i < 4; i++)
		points[i] = { collisionPoints[i].x + loc.x, collisionPoints[i].y + loc.y };

	col = std::make_unique<Fcollision>(points, ' ');
	collision::addCollisionObject(col.get());
}

void AautoFisher::calcIfPlayerInfront() {
	inFrontPlayer = GetCharacter()->getCharLoc().y >= loc.y + 6.f;
}

void AautoFisher::setStats() {
	// 1: .1, 100 = .01
	// updates the animation speed
	float y = -(1.f / 1100.f) * *level + 0.100909f;

	if (anim)
		anim->SetCurrAnimDuration(y);

	outline->SetCurrAnimDuration(y);
	fishingLine->SetCurrAnimDuration(y);

	// updates the max hold currency
	maxCurrency = *level * 100;
}

// takes input if the list isn't heldFish
double AautoFisher::calcCurrencyHeld(std::vector<FsaveFishData> fishList) {
	if (fishList.size() == 0)
		fishList = heldFish;

	double currency = 0;
	for (int i = 0; i < fishList.size(); i++) {
		FsaveFishData currSaveFish = fishList[i];
		FfishData currFish = SaveData::data.fishData[currSaveFish.id];
		currency += currSaveFish.numOwned[0] * upgrades::getFishSellPrice(currFish, 0);
	}

	return currency;
}

void AautoFisher::upgrade() {
	if (*level >= maxLevel)
		return;

	std::tuple<uint32_t, int, double> levelPrice = getUpgradeCost();
	auto& [upgradeCurrencyId, upgradeLevel, upgradePrice] = levelPrice;

	if (SaveData::saveData.currencyList.at(upgradeCurrencyId).numOwned < upgradePrice)
		return;

	// take currency
	SaveData::saveData.currencyList.at(upgradeCurrencyId).numOwned -= upgradePrice;
	*level += upgradeLevel;

	Main::currencyWidget->updateList();

	upgradeAnimIndex = floor(*level / 10);

	// update the autofisher animation
	setStats();

	// see if can fish again
	if (anim && calcCurrencyHeld() < maxCurrency && anim->IsStopped())
		anim->start();

	if (afMoreInfoUI && afMoreInfoUI->isVisible())
		afMoreInfoUI->updateUI();
}

std::tuple<uint32_t, int, double> AautoFisher::getUpgradeCost() {
	double currency = SaveData::saveData.currencyList.at(worldId).numOwned;

	int tempLevel = *level;
	if (multiplier != (int)INFINITY) {
		double cost = 0;
		for (int i = 0; i < multiplier; i++) {
			if (tempLevel + 1 > maxLevel)
				break;
			cost += price(tempLevel);
			tempLevel++;
		}

		if (UI)
			UI->buttonText->setText(shortNumbers::convert2Short(cost));

		return std::tuple(worldId, tempLevel - *level, cost);
	} else {
		double minPrice = price(*level);
		// return 1 if don't have enough money for 1 upgrade
		if (currency < minPrice) {
			if (UI)
				UI->buttonText->setText(shortNumbers::convert2Short(minPrice));
			return std::tuple(worldId, 1, minPrice);
		}

		double cost = 0;
		int num = 0;
		
		// if have enough money && isn't max level
		while (cost + price(num + *level) <= currency && *level + num < maxLevel) {
			//std::cout << "cost: " << cost << " + " << price(num + *level) << " <= " << currency << " && " << *level << " + " << num << " < " << maxLevel << std::endl;
			cost += price(num + *level);
			num++;
		}

		if (UI)
			UI->buttonText->setText(shortNumbers::convert2Short(cost));
		return std::tuple(worldId, num, cost);
	}
}

double AautoFisher::price(int level) {
	// 10 * 1.1^x
	return 10 * pow(1.1, level);
}

float AautoFisher::getCatchTime() {
	if (anim)
		return anim->GetCellNum().x * anim->GetCurrAnimDuration();
	return 4;
}

double AautoFisher::calcIdleProfits(double afkTime) {
	double currencyNum = 0;
	int numOfFishCatched = round(afkTime / getCatchTime());
	std::vector<std::pair<uint32_t, double>> fishList = calcAutoFishList(numOfFishCatched);
	for (int i = 0; i < fishList.size(); i++) {
		FfishData* currFish = &SaveData::data.fishData.at(fishList[i].first);
		currencyNum += fishList[i].second * upgrades::getFishSellPrice(*currFish, 0);
	}

	return currencyNum;
}

std::vector<std::pair<uint32_t, double>> AautoFisher::calcAutoFishList(int fishNum) {
	std::unordered_map<uint32_t, FfishData> fishList;
	std::vector<std::pair<uint32_t, double>> fishNumList;

	int fishingRodId = 0;
	if (fishingRodId == -1)
		fishingRodId = 0;

	// calc all fish in world
	for (auto& [key, value] : SaveData::data.fishData) {
		if (value.worldId == Scene::GetCurrWorldId() && fishingPower >= value.fishingPower) {
			fishList.insert({key, value});
		}
	}

	std::unordered_map<uint32_t, float> fishChance = calcIdleFishChance(fishList);

	for (auto& [key, value] : fishList) {
		int prob = fishChance.at(key) * (float)fishNum;
		std::pair<uint32_t, double> fishPair(key, prob);
		fishNumList.push_back(fishPair);
	}

	return fishNumList;
}

// calculates what fish the auto fisher can catch
std::unordered_map<uint32_t, float> AautoFisher::calcIdleFishChance(std::unordered_map<uint32_t, FfishData> fishList) {
	std::unordered_map<uint32_t, float> probList;
	float total = 0;

	for (auto& [key, value] : fishList) {
		total += value.probability;
	}

	for (auto& [key, value] : fishList) {
		probList.insert({ key, value.probability / total });
	}

	return probList;
}

void AautoFisher::startFishing() {
	if (anim && calcCurrencyHeld() < maxCurrency && anim->IsStopped())
		anim->start();
}

// fish per second
double AautoFisher::calcFPS() {
	// how fast it can catch fish
	return 1 / math::max(getCatchTime(), 0.00001f);
}

// money per second
double AautoFisher::calcMPS() {
	// fps
	// avg currency
	// avg fish cought

	std::vector<std::pair<uint32_t, float>> probList = calcFishProbability(SaveData::data.fishData, false);

	// total
	// price * percent

	float prevPercent = 0;
	float totalPrice = 0;
	for (int i = 0; i < probList.size(); i++) {
		// * %
		uint32_t id = probList[i].first;
		float percent = probList[i].second;

		totalPrice += (percent - prevPercent) * upgrades::getFishSellPrice(SaveData::data.fishData.at(id), 0);
		//std::cout << "percent: " << percent << ", price: " << ((percent - prevPercent) * upgrades::getFishSellPrice(SaveData::data.fishData[id])) << std::endl;

		prevPercent = percent;
	}

	return totalPrice * calcFPS();
}

void AautoFisher::OutlineUpdate(int frame) {
	if (outline)
		outline->SetCurrFrameLoc(vector(frame, 0));
}