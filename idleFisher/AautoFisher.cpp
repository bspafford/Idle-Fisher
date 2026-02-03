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
	worldId = Scene::GetCurrWorldId();

	if (!autoFisherSpriteSheet)
		autoFisherSpriteSheet = std::make_shared<Image>("images/autoFisher/autoFisher.png", vector{ 0, 0 }, true);
	if (!fishingLineSpriteSheet)
		fishingLineSpriteSheet = std::make_shared<Image>("images/autoFisher/fishingLine.png", vector{ 0, 0 }, true);
	if (!outlineSpriteSheet)
		outlineSpriteSheet = std::make_shared<Image>("images/autoFisher/outline.png", vector{ 0, 0 }, true);

	this->id = id;
	FautoFisherStruct& afData = SaveData::data.autoFisherData.at(id);
	loc = { afData.xLoc, afData.yLoc };

	setupCollision();
	
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
	anim->SetCurrFrameLoc(vector(math::randRangeInt(0, 39), 0)); // give random frame, so it looks better


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

	autoFisherNum = world::currWorld->autoFisherList.size();

	UI = std::make_unique<autoFisherUI>(nullptr, this, loc);

	afMoreInfoUI = std::make_unique<AFmoreInfoUI>(nullptr, this);

	recastTimer = CreateDeferred<Timer>();
	recastTimer->addCallback(this, &AautoFisher::Recast);
	recastAudio = std::make_unique<Audio>("recasts/G2.wav", anim->getLoc() + vector(anim->GetCellSize().x / 2.f, 0));
	numberWidget = std::make_unique<NumberWidget>(nullptr, true);

	setStats();
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

	if (numberWidget)
		numberWidget->draw(shaderProgram);
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
	for (auto& [id, data] : heldFish) {
		SaveData::saveData.fishData.at(id).numOwned[0] += data.numOwned[0];
		SaveData::saveData.fishData.at(id).totalNumOwned[0] += data.numOwned[0];
		data.numOwned[0] = 0;
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
	FfishData currFish;
	if (!calcFish(&currFish)) {
		anim->stop();
		return;
	}

	int catchNum = 1;

	// if there is enough room for the fish
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
	if (calcCurrencyHeld() + Upgrades::Get(StatContext(Stat::FishPrice, currFish.id, 0)) * catchNum <= maxCurrency) {
		if (catchNum > 0) {
			double recast = Upgrades::Get(Stat::RecastProcChance);
			if (!recastActive && math::randRange(0.0, 100.0) <= recast) // recast not active && should recast
				StartRecast(currFish.id, catchNum);

			FsaveFishData& saveFishData = heldFish[currFish.id];
			saveFishData.id = currFish.id;
			saveFishData.numOwned[0] += catchNum;
			saveFishData.totalNumOwned[0] += catchNum;

			numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), Upgrades::Get(StatContext(Stat::FishPrice, currFish.id)) * catchNum, NumberType::FishCaught);
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

bool AautoFisher::calcFish(FfishData* fishData) {
	float rand = math::randRange(0.f, 1.f);
	std::vector<std::pair<uint32_t, float>> probList = calcFishProbability(SaveData::data.fishData);

	for (int i = 0; i < probList.size(); i++) {
		if (rand <= probList[i].second) {
			if (fishData)
				*fishData = SaveData::data.fishData.at(probList[i].first);
			return true;
		}
	}

	return false;
}

std::vector<std::pair<uint32_t, float>> AautoFisher::calcFishProbability(const std::unordered_map<uint32_t, FfishData>& fishData, bool isCurrencyAFactor) {
	double fishingPower = Upgrades::Get(StatContext(Stat::AutoFisherPower, id));

	float totalProb = 0;
	double heldCurrency = calcCurrencyHeld();
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));

	// starts at 1 to skip premium
	for (auto& [fishId, fData] : fishData) {
		if (fishId == 1u) // premium fish id
			continue; // dont include premium here

		// see if autofisher has enough fishing power, see if theres enough room for the fish
		if (fData.fishingPower <= fishingPower && fData.worldId == Scene::GetCurrWorldId()) {
			if ((isCurrencyAFactor && heldCurrency + Upgrades::Get(StatContext(Stat::FishPrice, fishId, 0)) <= maxCurrency) || !isCurrencyAFactor)
				totalProb += float(fData.probability);
		}
	}
	std::vector<std::pair<uint32_t, float>> probList;
	float test = 0;

	// starts at 1 to skip premium
	for (auto& [fishId, fData] : fishData) {
		if (fishId == 1u)
			continue; // dont include premium here

		if (fData.fishingPower <= fishingPower && fData.worldId == Scene::GetCurrWorldId()) {
			if ((isCurrencyAFactor && heldCurrency + Upgrades::Get(StatContext(Stat::FishPrice, fishId, 0)) <= maxCurrency) || !isCurrencyAFactor) {
				test += fData.probability / totalProb;
				probList.push_back(std::pair{ fishId, test});
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
	UI->buttonText->setText(shortNumbers::convert2Short(Upgrades::GetPrice(id, upgradeAmount)));

	double animSpeed = Upgrades::Get(StatContext(Stat::AutoFisherSpeed, id));
	anim->SetCurrAnimDuration(animSpeed);

	SetUpgradeAnim();
}

// takes input if the list isn't heldFish
double AautoFisher::calcCurrencyHeld() {
	double currency = 0;
	for (auto& [fishId, saveFishData] : heldFish) {
		currency += saveFishData.numOwned[0] * Upgrades::Get(StatContext(Stat::FishPrice, fishId, 0));
	}

	return currency;
}

void AautoFisher::upgrade() {
	if (!Upgrades::LevelUp(id, upgradeAmount))
		return;

	setStats();

	// if auto fisher was full, see if it now has enough space to start again
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
	if (anim && calcCurrencyHeld() < maxCurrency && anim->IsStopped())
		anim->start();

	if (afMoreInfoUI && afMoreInfoUI->isVisible())
		afMoreInfoUI->updateUI();
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
		currencyNum += fishList[i].second * Upgrades::Get(StatContext(Stat::FishPrice, currFish->id, 0));
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
	double fishingPower = Upgrades::Get(StatContext(Stat::AutoFisherPower, id));
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
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
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

		totalPrice += (percent - prevPercent) * Upgrades::Get(StatContext(Stat::FishPrice, id, 0));
		prevPercent = percent;
	}

	return totalPrice * calcFPS();
}

void AautoFisher::OutlineUpdate(int frame) {
	if (outline)
		outline->SetCurrFrameLoc(vector(frame, 0));
	if (fishingLine)
		fishingLine->SetCurrFrameLoc(vector(frame, 0));
}

void AautoFisher::SetUpgradeAnim() {
	std::string name = std::to_string(SaveData::saveData.progressionData.at(id).level / 10 + 1);
	anim->setAnimation(name);
}

void AautoFisher::StartRecast(uint32_t fishId, double caughtNum) {
	recastActive = true;
	recastNum = 1;

	chainChance = Upgrades::Get(Stat::RecastChainChance);
	fishAtStartOfRecast = fishId;
	catchNumAtStartOfRecast = caughtNum;
	Recast();
}

void AautoFisher::Recast() {
	double caught = catchNumAtStartOfRecast * recastNum;

	FsaveFishData& fishData = SaveData::saveData.fishData.at(fishAtStartOfRecast);
	fishData.unlocked = true;
	fishData.numOwned[0] += caught;
	fishData.totalNumOwned[0] += caught;

	numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), recastNum, NumberType::Recast);

	recastAudio->SetPitch(std::powf(2.f, recastNum / 12.f)); // increase pitch by 1 note
	recastAudio->Play();

	if (math::randRange(0.0, 100.0) <= chainChance) { // should continue chain
		chainChance *= Upgrades::Get(Stat::RecastFalloff); // reduce chance for next go
		recastTimer->start(0.25f); // arbitrary time between each recast effect
		recastNum++;
	} else
		recastActive = false;
}