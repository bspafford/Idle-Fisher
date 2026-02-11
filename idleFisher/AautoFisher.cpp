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
#include "achievement.h"

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
	if (!fullnessLightSpriteSheet)
		fullnessLightSpriteSheet = std::make_shared<Image>("images/autoFisher/fullnessLight.png", vector{ 0, 0 }, true);
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

	// fullness light animation
	std::unordered_map<std::string, animDataStruct> fullnessLightData;
	fullnessLightData.insert({ "0", animDataStruct({0, 0}, {39, 0}, true) }); // green
	fullnessLightData.insert({ "1", animDataStruct({0, 1}, {39, 1}, true) }); // orange
	fullnessLightData.insert({ "2", animDataStruct({0, 2}, {39, 2}, true) }); // red
	fullnessLight = std::make_unique<animation>(fullnessLightSpriteSheet, 6, 9, fullnessLightData, true, loc + vector{ 13, 37 });
	fullnessLight->setAnimation("0");

	autoFisherNum = world::currWorld->autoFisherList.size();

	UI = std::make_unique<autoFisherUI>(nullptr, this, loc);

	afMoreInfoUI = std::make_unique<AFmoreInfoUI>(nullptr, this);

	vector soundLoc = anim->getLoc() + vector(anim->GetCellSize().x / 2.f, 0);

	recastTimer = CreateDeferred<Timer>();
	recastTimer->addCallback(this, &AautoFisher::Recast);
	recastAudio = std::make_unique<Audio>("recasts/1.wav", AudioType::SFX, soundLoc);
	numberWidget = std::make_unique<NumberWidget>(nullptr, true);

	castAudio = std::make_unique<Audio>("whoosh.wav", AudioType::SFX, soundLoc);
	steamAudio = std::make_unique<Audio>("steam.wav", AudioType::SFX, soundLoc);
	splashAudio = std::make_unique<Audio>("splash.wav", AudioType::SFX, soundLoc);
	reelAudio = std::make_unique<Audio>("temp/autoFisher/reel.wav", AudioType::SFX, soundLoc);
	machineAudio = std::make_unique<Audio>("machineNoise.wav", AudioType::SFX, soundLoc);
	drawerAudio = std::make_unique<Audio>("temp/autoFisher/drawer.wav", AudioType::SFX, soundLoc);
	catchAudio = std::make_unique<Audio>("pop.wav", AudioType::SFX, soundLoc);
	machineAudio->Play(true);

	setStats();
}

AautoFisher::~AautoFisher() {
	collision::removeCollisionObject(col.get());
}

void AautoFisher::Update(float deltaTime) {
	calcIfPlayerInfront();

	bool canReach = GetCharacter()->CanPlayerReach(anim->getLoc() + vector(anim->GetCellSize().x, 0.f));
	bMouseOver = canReach && anim->IsMouseOver(true);

	if (!canReach)
		return;

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
	if (fullnessLight)
		fullnessLight->draw(shaderProgram);
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

void AautoFisher::collectFish() {
	for (auto& [id, data] : heldFish) {
		SaveData::saveData.fishData.at(id).numOwned[0] += data.numOwned[0];
		SaveData::saveData.fishData.at(id).totalNumOwned[0] += data.numOwned[0];
		data.numOwned[0] = 0;
	}

	SaveData::saveData.autoFisherList.at(id).fullness = 0;
	SetFullnessIndex();
	Main::heldFishWidget->updateList(true);

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

	SetFullnessIndex();

	double catchNum = Upgrades::Get(Stat::CatchNum) / 100.0;
	double caught = floor(catchNum); // guarenteed caught
	double remainder = catchNum - caught; // percent remainder
	if (math::randRange(0.0, 1.0) < remainder) // random check to see if player got rolled over value
		caught++;

	numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), Upgrades::Get(StatContext(Stat::FishPrice, currFish.id)) * caught, NumberType::FishCaught);
	
	// if there is enough room for the fish
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
	if (calcCurrencyHeld() + Upgrades::Get(StatContext(Stat::FishPrice, currFish.id, 0)) * caught <= maxCurrency) {
		if (caught > 0) {
			double recast = Upgrades::Get(Stat::RecastProcChance);
			if (!recastActive && math::randRange(0.0, 100.0) <= recast) // recast not active && should recast
				StartRecast(currFish.id, caught);

			FsaveFishData& saveFishData = heldFish[currFish.id];
			saveFishData.id = currFish.id;
			saveFishData.numOwned[0] += caught;
			saveFishData.totalNumOwned[0] += caught;

		}

		double currencyHeld = calcCurrencyHeld();
		SaveData::saveData.autoFisherList.at(id).fullness = currencyHeld;
		if (anim && currencyHeld >= maxCurrency) {
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

std::vector<std::pair<uint32_t, float>> AautoFisher::calcFishProbability(const std::unordered_map<uint32_t, FfishData>& fishData, bool isCurrencyAFactor, double customCurrency) {
	double fishingPower = Upgrades::Get(StatContext(Stat::AutoFisherPower, id));

	float totalProb = 0;
	double heldCurrency = customCurrency;
	if (customCurrency == -1)
		heldCurrency = calcCurrencyHeld();
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));

	// starts at 1 to skip premium
	for (auto& [fishId, fData] : fishData) {
		if (fishId == 1u || fData.isRareFish) // premium fish id || rare
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
		if (fishId == 1u || fData.isRareFish)
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

	SetUpgradeAnim();

	double animSpeed = Upgrades::Get(StatContext(Stat::AutoFisherSpeed, id));
	anim->SetCurrAnimDuration(animSpeed);
}

// takes input if the list isn't heldFish
double AautoFisher::calcCurrencyHeld() {
	double currency = 0;
	for (auto& [fishId, saveFishData] : heldFish) {
		currency += saveFishData.numOwned[0] * Upgrades::Get(StatContext(Stat::FishPrice, fishId, 0));
	}

	SaveData::saveData.autoFisherList.at(id).fullness = currency; // update fullness for save data

	return currency;
}

void AautoFisher::upgrade() {
	if (!Upgrades::LevelUp(id, upgradeAmount))
		return;

	SetFullnessIndex();
	setStats();

	// if auto fisher was full, see if it now has enough space to start again
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
	if (anim && calcCurrencyHeld() < maxCurrency && anim->IsStopped())
		anim->start();

	if (afMoreInfoUI && afMoreInfoUI->isVisible())
		afMoreInfoUI->updateUI();

	Achievements::CheckGroup(AchievementTrigger::CurrencyPerSecond);
	Achievements::CheckGroup(AchievementTrigger::UpgradeAutoFisher);
}

float AautoFisher::getCatchTime() {
	if (anim)
		return anim->GetCellNum().x * anim->GetCurrAnimDuration();
	return 4;
}

std::vector<std::pair<uint32_t, double>> AautoFisher::calcAutoFishList(int fishNum) {
	std::unordered_map<uint32_t, FfishData> fishList;
	std::vector<std::pair<uint32_t, double>> fishNumList;

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
	for (auto& [key, value] : fishList)
		total += value.probability;

	for (auto& [key, value] : fishList)
		probList.insert({ key, value.probability / total });

	return probList;
}

void AautoFisher::startFishing() {
	double maxCurrency = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));

	SetFullnessIndex();

	if (anim && calcCurrencyHeld() < maxCurrency && anim->IsStopped())
		anim->start();
}

double AautoFisher::calcFPS() {
	double catchNum = Upgrades::Get(Stat::CatchNum) / 100.0;
	return 1 / math::max(getCatchTime(), 0.00001f) * catchNum;
}

double AautoFisher::calcMPS(double customCurrency) {
	// fps
	// avg currency
	// avg fish cought

	std::vector<std::pair<uint32_t, float>> probList = calcFishProbability(SaveData::data.fishData, false, customCurrency);

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
	if (fullnessLight)
		fullnessLight->SetCurrFrameLoc(vector(frame, fullnessIndex));

	if (frame == 6) {
		steamAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		steamAudio->Play();
	} else if (frame == 8) {
		castAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		castAudio->Play();
	} else if (frame == 26) {
		drawerAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		drawerAudio->Play();
		reelAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		reelAudio->Play();
	} else if (frame == 29) {
		splashAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		splashAudio->Play();
	} else if (frame == 37) {
		drawerAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		drawerAudio->Play();
	} else if (frame == 40) {
		catchAudio->SetSpeed(math::randRange(0.9f, 1.1f));
		catchAudio->Play();
	}
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

	recastAudio->SetAudio("recasts/" + std::to_string(std::min(recastNum, 5)) + ".wav");
	recastAudio->Play();

	if (math::randRange(0.0, 100.0) <= chainChance) { // should continue chain
		chainChance *= Upgrades::Get(Stat::RecastFalloff); // reduce chance for next go
		recastTimer->start(0.25f); // arbitrary time between each recast effect
		recastNum++;
	} else
		recastActive = false;
}

void AautoFisher::FillWithRandomFish() {
	heldFish.clear();

	double fullness = SaveData::saveData.autoFisherList.at(id).fullness;

	// loop throguh all auto fishers in world
	// add up all their probabilities / total
	std::unordered_map<uint32_t, float> probabilities;
	float totalProb = 0.f;
	std::vector<std::pair<uint32_t, float>> probability = calcFishProbability(SaveData::data.fishData, false, fullness);
	for (auto& [id, prob] : probability) {
		probabilities[id] += prob;
		totalProb += prob;
	}

	// calculate fish
	for (auto& [id, prob] : probabilities) {
		double fishPrice = Upgrades::Get(StatContext(Stat::FishPrice, id));
		float percent = prob / totalProb;
		double fishNum = percent * (fullness / fishPrice);

		if (fishNum < 1.0)
			continue;

		FsaveFishData saveFishData;
		saveFishData.id = id;
		saveFishData.numOwned[0] = fishNum;

		heldFish.insert({ id, saveFishData });
	}
	
	// see if the auto fisher has enough room to start fishing
	SetFullnessIndex();
	if (calcFish(nullptr)) {
		startFishing();
		anim->SetCurrFrameLoc(vector(math::randRangeInt(0, 39), anim->GetCurrFrameLoc().y)); // give random frame, so it looks better
	}
}

void AautoFisher::SetFullnessIndex() {
	// 0-75%  = green
	// 75-95% = orange
	// 95%+   = red
	double maxCapacity = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, id));
	double percent = calcCurrencyHeld() / maxCapacity;
	if (percent <= 0.75f) // green
		fullnessIndex = 0;
	else if (percent <= 0.95) // orange
		fullnessIndex = 1;
	else
		fullnessIndex = 2;

	fullnessLight->setAnimation(std::to_string(fullnessIndex));
}