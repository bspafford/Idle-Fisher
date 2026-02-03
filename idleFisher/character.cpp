#include "character.h"

#include <iostream>

#include "main.h"
#include "Input.h"
#include "Cursor.h"
#include "shaderClass.h"
#include "Rectangle.h"
#include "worlds.h"
#include "AautoFisher.h"
#include "collision.h"
#include "timer.h"
#include "fishSchool.h"
#include "upgrades.h"
#include "achievement.h"

// widget
#include "fishComboWidget.h"
#include "comboWidget.h"
#include "currencyWidget.h"
#include "fishUnlocked.h"
#include "heldFishWidget.h"
#include "premiumBuffWidget.h"
#include "UIWidget.h"
#include "comboOvertimeWidget.h"
#include "achievementWidget.h"
#include "newRecordWidget.h"
#include "NumberWidget.h"

#include "debugger.h"

Acharacter* GetCharacter() {
	return Acharacter::characterPtr;
}

Acharacter::Acharacter() {
	if (!characterPtr)
		characterPtr = this;

	float duration = 0.13f;
	std::unordered_map<std::string, animDataStruct> animData;

	// walking
	animData.insert({ "walkE", animDataStruct({0, 0}, {5, 0}, true, duration) });
	animData.insert({ "walkNE", animDataStruct({6, 0}, {11, 0}, true, duration) });
	animData.insert({ "walkN", animDataStruct({12, 0}, {17, 0}, true, duration) });
	animData.insert({ "walkNW", animDataStruct({18, 0}, {23, 0}, true, duration) });
	animData.insert({ "walkW", animDataStruct({0, 1}, {5, 1}, true, duration) });
	animData.insert({ "walkSW", animDataStruct({6, 1}, {11, 1}, true, duration) });
	animData.insert({ "walkS", animDataStruct({12, 1}, {17, 1}, true, duration) });
	animData.insert({ "walkSE", animDataStruct({18, 1}, {23, 1}, true, duration) });

	// idle
	animData.insert({ "idleE", animDataStruct({0, 2}, {13, 2}, true, duration) });
	animData.insert({ "idleNE", animDataStruct({14, 2}, {27, 2}, true, duration) });
	animData.insert({ "idleN", animDataStruct({0, 3}, {13, 3}, true, duration) });
	animData.insert({ "idleNW", animDataStruct({14, 3}, {27, 3}, true, duration) });
	animData.insert({ "idleW", animDataStruct({0, 4}, {13, 4}, true, duration) });
	animData.insert({ "idleSW", animDataStruct({14, 4}, {27, 4}, true, duration) });
	animData.insert({ "idleS", animDataStruct({0, 5}, {13, 5}, true, duration) });
	animData.insert({ "idleSE", animDataStruct({14, 5}, {27, 5}, true, duration) });

	// fishing
	animData.insert({ "castSE", animDataStruct({0, 6}, {17, 6}, false) });
	animData.insert({ "idleFishingSE", animDataStruct({18, 6}, {31, 6}, true) });
	animData.insert({ "transitionSE", animDataStruct({0, 7}, {2, 7}, false) });
	animData.insert({ "waitSE", animDataStruct({3, 7}, {6, 7}, true) });
	animData.insert({ "pullSE", animDataStruct({7, 7}, {18, 7}, false) });

	// fishing rod
	std::unordered_map<std::string, animDataStruct> fishingRodData;
	fishingRodData.insert({ "castSE", animDataStruct({0, 0}, {17, 0}, false) });
	fishingRodData.insert({ "idleFishingSE", animDataStruct({18, 0}, {31, 0}, true) });
	fishingRodData.insert({ "transitionSE", animDataStruct({32, 0}, {34, 0}, false) });
	fishingRodData.insert({ "waitSE", animDataStruct({35, 0}, {38, 0}, true) });
	fishingRodData.insert({ "pullSE", animDataStruct({39, 0}, {51, 0}, false) });

	fishingRod = std::make_unique<animation>("character/fishingRod.png", 108, 83, fishingRodData, true);
	
	anim = std::make_unique<animation>("character/characterSpriteSheet.png", 21, 49, animData, true, vector{ 0, 0 });
	anim->setAnimation("idleSE");
	anim->addFinishedCallback(this, &Acharacter::animFinished);
	anim->addFrameCallback(this, &Acharacter::setFishingTipLoc);
	anim->start();

	bobberCatchTimer = CreateDeferred<Timer>();
	bobberCatchTimer->addCallback(this, &Acharacter::bobberCatchAnim);
	bobberCatchTimer->addUpdateCallback(this, &Acharacter::bobberCatchAnim);
	bobberBobTimer = CreateDeferred<Timer>();
	bobberBobTimer->addCallback(this, &Acharacter::bobberBobAnim);
	bobberBobTimer->addUpdateCallback(this, &Acharacter::bobberBobAnim);

	bobberImg = std::make_unique<Image>("images/bobber.png", vector{ 100, 100 }, true);
	bobberImg->SetPivot({ 0.5f, 0.5f });

	std::unordered_map<std::string, animDataStruct> bobberData;
	bobberData.insert({ "water", animDataStruct({0, 0}, {3, 0}, true, .3f) });
	bobberWaterAnimFront = std::make_unique<animation>("bobberWaterAnimFront.png", 15, 12, bobberData, true, vector{ 0, 0 });
	bobberWaterAnimFront->SetPivot({ 0.5f, 0.5f });
	bobberWaterAnimFront->setAnimation("water");
	bobberWaterAnimFront->start();
	bobberWaterAnimBack = std::make_unique<animation>("bobberWaterAnimBack.png", 15, 12, bobberData, true, vector{ 0, 0 });
	bobberWaterAnimBack->SetPivot({ 0.5f, 0.5f });
	bobberWaterAnimBack->setAnimation("water");
	bobberWaterAnimBack->start();


	setPlayerColPoints();

	premiumCatchTimer = CreateDeferred<Timer>();
	premiumCatchTimer->addCallback(this, &Acharacter::setCatchPremium);

	comboOvertimeWidget = std::make_unique<UcomboOvertimeWidget>(nullptr);
	comboOvertimeWidget->addFinishedCallback(this, &Acharacter::comboOvertimeFinished);

	// Audio
	catchFishAudio = std::make_unique<Audio>("pop.wav");

	recastTimer = CreateDeferred<Timer>();
	recastTimer->addCallback(this, &Acharacter::Recast);
	recastAudio = std::make_unique<Audio>("recasts/G2.wav");
	numberWidget = std::make_unique<NumberWidget>(nullptr, true);
}

void Acharacter::animFinished() {
	if (anim->GetCurrAnim().find("cast") != std::string::npos) { // if not cast
		// change to next animation
		anim->setAnimation("idleFishingSE");
		anim->start();
		fishingRod->setAnimation("idleFishingSE");
		fishingRod->start();
	} else if (anim->GetCurrAnim() == "transitionSE") {
		anim->setAnimation("waitSE");
		anim->start();
		fishingRod->setAnimation("waitSE");
		fishingRod->start();
	} else if (anim->GetCurrAnim() == "pullSE") {
		anim->setAnimation("castSE");
		anim->start();
		fishingRod->setAnimation("castSE");
		fishingRod->start();
	}
}

void Acharacter::setFishingTipLoc(int frame) {
	vector loc = anim->getAbsoluteLoc() - anim->GetCellSize() / 2.f + vector{ -34.f, 11.f };
	if (anim->GetCurrAnim() == "castSE") {
		fishingTipLoc = castAnimLocs[frame] + loc;
		if (frame >= 11)
			showFishingLine = true;
	} else if (anim->GetCurrAnim() == "idleSE") {
		fishingTipLoc = idleFishing[frame] + loc;
		showFishingLine = true;
	} else if (anim->GetCurrAnim() == "transitionSE") {
		fishingTipLoc = transition[frame] + loc;
		showFishingLine = true;
	} else if (anim->GetCurrAnim() == "waitSE") {
		fishingTipLoc = pullAnim[frame] + loc;
		showFishingLine = true;
	} else if (anim->GetCurrAnim() == "pullSE") {
		fishingTipLoc = catchAnim[frame] + loc;
		if (frame >= 4)
			showFishingLine = false;
	}
}

void Acharacter::move(float deltaTime) {
	// Handles key inputs
	moveDir = { 0 , 0 };
	if (Input::getKeyHeld(GLFW_KEY_W))
		moveDir += vector{ 0, 1 };
	if (Input::getKeyHeld(GLFW_KEY_A))
		moveDir += vector{ -1, 0 };
	if (Input::getKeyHeld(GLFW_KEY_S))
		moveDir += vector{ 0, -1 };
	if (Input::getKeyHeld(GLFW_KEY_D))
		moveDir += vector{ 1, 0 };

	if (fishingStop && moveDir.x == 0 && moveDir.y == 0)
		fishingStop = false;

	if (fishingStop)
		return;

	if (isFishing && (moveDir.x != 0 || moveDir.y != 0))
		stopFishing();

	if (!canMove || (moveDir.x == 0 && moveDir.y == 0))
		return;

	// if should use collision // debugging
	if (true)
		collision::TestCollision(col.get(), moveDir, deltaTime);
	else {
		vector normDir = math::normalize(moveDir);
		SaveData::saveData.playerLoc.x += normDir.x * speed * deltaTime;
		SaveData::saveData.playerLoc.y += normDir.y * speed * deltaTime;
	}
	setPlayerColPoints();
}

void Acharacter::setPlayerColPoints() {
	float radius = 5;
	vector loc = getCharLoc();

	col = std::make_unique<Fcollision>(loc, radius, ' ');
}

void Acharacter::draw(Shader* shaderProgram) {
	// if bobber above player, render behind
	bool bobberBehind = false;
	if (tempBobberLoc.y > math::worldToScreen(anim->getLoc()).y) {
		bobberBehind = true;
		drawFishingLine(shaderProgram);
	}

	vector animLoc = SaveData::saveData.playerLoc - anim->GetCellSize() / 2.f;
	anim->setLoc(animLoc);
	anim->draw(shaderProgram);
	if (isFishing) {
		fishingRod->setLoc(animLoc + vector{ -44.f, 0.f });
		fishingRod->draw(shaderProgram);
	}

	if (!bobberBehind)
		drawFishingLine(shaderProgram);

	numberWidget->draw(shaderProgram);
}

void Acharacter::DrawWidgets(Shader* shaderProgram) {
	comboOvertimeWidget->draw(shaderProgram);
}

vector Acharacter::getCharLoc() {
	return SaveData::saveData.playerLoc - vector{ 0.f, anim->GetCellSize().y / 2.f };
}

void Acharacter::Update(float deltaTime) {
	move(deltaTime);

	if (Input::getMouseButtonDown(MOUSE_BUTTON_LEFT))
		leftClick();
	else if (Input::getMouseButtonDown(MOUSE_BUTTON_RIGHT))
		stopFishing();

	// animate character depending on move direction
	if (!isFishing) {
		std::vector<std::string> walkAnimList = { "walkSW", "walkS", "walkSE", "walkE", "walkNE", "walkN", "walkNW", "walkW" };
		std::vector<std::string> idleAnimList = { "idleSW", "idleS", "idleSE", "idleE", "idleNE", "idleN", "idleNW", "idleW" };

		if (moveDir.x != 0 || moveDir.y != 0) {
			prevMove = moveDir;
			float angle = atan2(moveDir.y, moveDir.x) * 180.f / M_PI;
			int y = static_cast<int>(floor(1.f / 45.f * (angle + 45.f / 2.f))) + 3;
			if (anim->GetCurrAnim() != walkAnimList[y])
				anim->setAnimation(walkAnimList[y]);
		} else {
			float angle = atan2(prevMove.y, prevMove.x) * 180 / M_PI;
			int y = static_cast<int>(floor(1.f / 45.f * (angle + 45.f / 2.f))) + 3;
			if (anim->GetCurrAnim() != idleAnimList[y])
				anim->setAnimation(idleAnimList[y]);
		}
	}
}

void Acharacter::leftClick() {
	if (widget::getCurrWidget())
		return;

	// puts bobber in water
	if (Cursor::getMouseOverWater() && Upgrades::Get(Stat::Power) > 0 && !isFishing) { // if power is > 0
		Input::setLeftClick(this, &Acharacter::StartFishing, false);
	// catch fish
	} else if (isFishing && Main::fishComboWidget->isVisible()) {
		showFish = false;

		// catch fish
		// dont need to check if combo unlocked, the upgrade class takes care of that
		int combo = Main::fishComboWidget->getCombo();
		switch (combo) {
		case 0:
			if (!Upgrades::Get(Stat::ShouldResetCombo)) // reset combo if false
				comboNum = Upgrades::Get(StatContext(Stat::ComboReset, comboNum));
			break;
		// case 1: // stays the same
		case 2:
			double comboMax = Upgrades::Get(Stat::ComboMax);
			comboNum = math::clamp(comboNum + Upgrades::Get(Stat::ComboIncrease), Upgrades::Get(Stat::ComboMin), comboMax);
			break;
		}

		comboNum = round(comboNum);
		Main::comboWidget->spawnComboNumber();

		catchFishAudio->Play();

		// set animation
		anim->setAnimation("pullSE");
		anim->start();
		fishingRod->setAnimation("pullSE");
		fishingRod->start();

		// set fish image
		if (!fishImg) {
			fishImg = std::make_unique<Image>(currFish.thumbnail.c_str(), bobberLoc, true);
			fishImg->SetPivot({ 0.5f, 0.5f });
		} else
			fishImg->setImage(currFish.thumbnail.c_str());

		// add fish
		if (currFish.id != 1u) { // if not premium
			FsaveFishData& saveFishData = SaveData::saveData.fishData[currFish.id];
			if (!saveFishData.unlocked)
				Main::fishUnlocked->start(currFish);

			if (saveFishData.biggestSizeCaught < currFishSize) {
				saveFishData.biggestSizeCaught = currFishSize;
				if (saveFishData.unlocked)
					Main::newRecordWidget->start(currFishSize);
			}

			double catchNum = Upgrades::Get(Stat::CatchNum) / 100.0; // 387 / 100 = 3.87
			double caught = floor(catchNum); // guarenteed caught // 3
			double remainder = catchNum - caught; // percent remainder // 3.87 - 3 = 0.87
			if (math::randRange(0.0, 1.0) < remainder) // random check to see if player got rolled over value
				caught++;

			if (caught > 0) { // can catch no fish
				double recast = Upgrades::Get(Stat::RecastProcChance);
				if (!recastActive && math::randRange(0.0, 100.0) <= recast) // recast not active && should recast
					StartRecast(currFish.id, caught);

				saveFishData.unlocked = true;
				saveFishData.numOwned[currFishQuality] += caught;
				saveFishData.totalNumOwned[currFishQuality] += caught;
				showFish = true;

				numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), Upgrades::Get(StatContext(Stat::FishPrice, currFish.id)) * catchNum, NumberType::FishCaught);
			}

		} else { // if premium
			SaveData::saveData.currencyList.at(50u).numOwned++;
			// give the player buffs
			// instance cash, instantly collect currency
			// low long buff, get more fish on catch
			// high fast buff, get more fish on catch
			premiumFishBuff();
			// start premium fish cooldown
			premiumCatchTimer->start(Upgrades::Get(Stat::PremiumCoolDownTime));
			canCatchPremium = false;

			Main::currencyWidget->updateList();

			showFish = true;
		}

		achievement::checkAchievements();

		// updates held fish widget
		Main::heldFishWidget->updateList();

		// check if fishing rod is inside a fishSchool or not!
		calcFishSchool();

		// start up fishing again
		fishing();

		bobberBobTimer->stop();
		bobberCatchTimer->start(stuff::animSpeed * 4.f);
	}
}

void Acharacter::fishing() {
	if (!isFishing || !canCatchWorldFish())
		return;

	// Show bobber combo
	currFish = calcFish(currFishQuality, currFishSize);

	Main::fishComboWidget->SetFish(currFish, currFishQuality);

	anim->setAnimation("transitionSE");
	anim->start();
	fishingRod->setAnimation("transitionSE");
	fishingRod->start();
}

FfishData Acharacter::calcFish(int& quality, int& fishSize) {
	float rand = math::randRange(0.f, 1.f);
	std::vector<std::pair<uint32_t, double>> probList = calcFishProbability(SaveData::data.fishData);

	for (int i = 0; i < probList.size(); i++)
		if (rand <= probList[i].second) {
			double randQuality = math::randRange(0.0, 100.0);
			if (randQuality <= 1.0) // gold 1%
				quality = 3;
			else if (randQuality <= 6.0) // silver 5%
				quality = 2;
			else if (randQuality <= 18.0) // bronze 12%
				quality = 1;
			else
				quality = 0;

			FfishData caughtFish = SaveData::data.fishData[probList[i].first];

			fishSize = math::randRangeInt(caughtFish.minSize, caughtFish.maxSize);
			return caughtFish;
		}

	// shouldn't hit this
	return (*SaveData::data.fishData.begin()).second;
}

// calculates the chance of each fish
// normalizes the value of the golden fish to always be its chance
// if its chance is .05 its always .05 even if another fish is 45% chance
// old method: premium = 1%, fish = 49%, actual premium chance = 2%, actual fish = 98%
// this method: premium = 1%, fish = 49%, actual premium chance = 1% actual fish = 99%
std::vector<std::pair<uint32_t, double>> Acharacter::calcFishProbability(const std::unordered_map<uint32_t, FfishData>& fishData) {
	// get premium currency chance
	// then do (100 - premium chance) / 100
	// calc fish chance and multuply by that number
	// then add the premium chance at the end
	std::vector<float> petBuff = { 1, 1, 1, 1, 1 }; // petBuffs::increaseChanceOfHigherFish();

	double premiumChance = canCatchPremium ? Upgrades::Get(Stat::PremiumCatchChance) : 0.0;
	float totalProb = 0; // premiumChance;
	int index = 0;
	for (auto [key, value] : fishData) {
		if (value.fishingPower <= Upgrades::Get(StatContext(Stat::Power)) && (value.worldId == Scene::GetCurrWorldId() || value.worldId == 1u)) {
			float val = value.probability;
			if (index < petBuff.size())
				val *= petBuff[index];
			totalProb += val;
		}

		++index;
	}

	double multiplier = (1.0 - (premiumChance / 100.0));

	std::vector<std::pair<uint32_t, double>> probList;
	double percent = 0.0;
	int index1 = 0;
	for (auto [key, value] : fishData) {
		if (value.fishingPower <= Upgrades::Get(StatContext(Stat::Power)) && (value.worldId == Scene::GetCurrWorldId() || value.worldId == 1u)) {
			double multi = 1.0;
			if (index1 < petBuff.size())
				multi = petBuff[index1];
			percent += value.probability / totalProb * multiplier * multi;
			probList.push_back(std::pair<uint32_t, double>{value.id, percent});
		}

		++index1;
	}

	if (canCatchPremium)
		probList.push_back({ std::pair<uint32_t, double>{ 1u, 1.0 } }); // premium

	return probList;
}

void Acharacter::StartFishing() {
	fishingStop = true;

	comboOvertimeWidget->Refill();

	isFishing = true;
	canMove = false;
	bobberLoc = math::screenToWorld(Input::getMousePos());
	tempBobberLoc = bobberLoc;

	bobberBobTimer->start(bobTime);

	// check if fishing rod is inside a fishSchool or not!
	calcFishSchool();

	// give upgrades
		// decrease fish time
		// increase catch num?

	Main::fishComboWidget->Start();

	fishing();

	// face the character the correct direction
	// start fishing animation
	vector fishRodPoint = stuff::screenSize / (stuff::pixelSize * 2.f);
	vector diff = math::normalize(bobberLoc - fishRodPoint);
	float angle = atan2(diff.y, diff.x) * 180.f / M_PI;
	int y = static_cast<int>(floor(1.f / 45.f * (angle + 45.f / 2.f)) + 3.5f);

	// set fishing anim
	//if (anim->animList != idleAnimWheel[y])
	showFishingLine = false;

	if (anim->GetCurrAnim().find("cast") == std::string::npos) { // if not cast animation, from any direction
		anim->setAnimation("castSE");
		anim->start();
		fishingRod->setAnimation("castSE");
		fishingRod->start();
	}
	//anim->setAnimation(idleAnimWheel[y], -1, true);
}

void Acharacter::stopFishing() {
	setCanMove(true);

	Main::fishComboWidget->Stop();

	isFishing = false;

	bobberBobTimer->stop();

	if (comboNum > 1) {
		comboOvertimeWidget->Start(1.f);
	} else
		Main::comboWidget->hideComboText();
}

void Acharacter::comboOvertimeFinished() {
	// remove widget
	comboOvertimeWidget->setVisibility(false);
	comboNum = Upgrades::Get(Stat::ComboMin);
	Main::comboWidget->hideComboText();
}

void Acharacter::premiumFishBuff() {
	// if get in green give more
	// if in fish school give more

	float rand = math::randRange(0.f, 1.f);

	float cashPercent = 0.45f;
	float lowLongBuffPercent = 0.45f + cashPercent;
	// float highShortBuffPercent = 0.10f + lowLongBuffPercent;

	if (rand <= cashPercent) { // instant cash
		// % from bank, or a certian amount of time, which ever is less

		// calc mps
		double mps = 0;
		for (std::unique_ptr<AautoFisher>& autoFisher : world::currWorld->autoFisherList)
			mps += autoFisher->calcMPS();

		FsaveCurrencyStruct& currencyStruct = SaveData::saveData.currencyList.at(Scene::GetCurrWorldId());

		// calc 10% of currency + held fish
		double heldCurrency = currencyStruct.numOwned;
		for (auto& [fishId, fishData] : SaveData::data.fishData) {
			if (fishData.worldId == Scene::GetCurrWorldId()) {
				FsaveFishData* saveFish = &SaveData::saveData.fishData.at(fishId);
				heldCurrency += saveFish->numOwned[0] * Upgrades::Get(StatContext(Stat::FishPrice, fishId, 0));
			}
		}

		double min = math::min(mps * 900.0, heldCurrency * 0.15);
		double currency = round(min + 10); // + 10 for base value incase you have no held currency or autofishers
		currencyStruct.numOwned += currency;
		currencyStruct.totalNumOwned += currency;

		numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), currency, NumberType::PremiumCash);

		Main::currencyWidget->updateList();
	} else if (rand <= lowLongBuffPercent) { // low long buff
		FgoldenFishStruct& premiumData = SaveData::data.goldenFishData.at(3u);
		Main::premiumBuffList.push_back(std::make_unique<UpremiumBuffWidget>(nullptr, premiumData)); // low long
		Main::UIWidget->setupLocs();

		numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), premiumData.multiplier, NumberType::PremiumBuff);

	} else { // high short buff
		FgoldenFishStruct& premiumData = SaveData::data.goldenFishData.at(4u);
		Main::premiumBuffList.push_back(std::make_unique<UpremiumBuffWidget>(nullptr, premiumData)); // high short
		Main::UIWidget->setupLocs();

		numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), premiumData.multiplier, NumberType::PremiumBuff);
	}
}

// tries to set canmove but will not work if something like a widget is active
void Acharacter::setCanMove(bool move) {
	if (move && widget::getCurrWidget() == NULL) {
		canMove = true;
	} else if (!move)
		canMove = false;
}

bool Acharacter::getCanMove() {
	return canMove;
}

float Acharacter::GetSpeed() {
	return speed;
}

Fcollision* Acharacter::GetCollision() {
	return col.get();
}

vector Acharacter::GetMoveDir() {
	return moveDir;
}

bool Acharacter::GetIsFishing() {
	return isFishing;
}

// straight line to character
void Acharacter::bobberCatchAnim() {
	int pullFrames = 4;

	vector goToLoc = anim->getAbsoluteLoc() + vector{ anim->GetCellSize().x / 2.f, anim->GetCellSize().y + 10.f };

	float time = bobberCatchTimer->getTime();
	float timer = bobberCatchTimer->getMaxTime();

	float percent = floor(time / timer * pullFrames) / pullFrames;

	float x = math::lerp(tempBobberLoc.x, goToLoc.x, percent);
	float y = math::lerp(tempBobberLoc.y, goToLoc.y, percent);

	bobberLoc = { x, y };
	fishImg->setLoc({ x, y });

	if (bobberCatchTimer->IsFinished()) {
		// plays pop sound when gets to charcter
		//Mix_PlayChannel(-1, sounds::catchSound, 0);

		showFishingLine = false;
		bobberBobTimer->start(bobTime);
		showFish = false;
	}
}

void Acharacter::bobberBobAnim() {
	float time = bobberBobTimer->getTime();
	float timer = bobberBobTimer->getMaxTime();

	// if reeling in fish
	if (anim->GetCurrAnim().find("wait") != std::string::npos || anim->GetCurrAnim().find("pull") != std::string::npos) {
		float percent = time / timer;
		int num = static_cast<int>(floor(percent * 14.f));
		bobberLoc = tempBobberLoc + vector{ 0.f, static_cast<float>(num % 2) };

	} else {
		vector bobberY(0, roundf(sin(time / timer * 2.f * M_PI)));
		bobberLoc = tempBobberLoc + bobberY;
	}

	// restarts the timer
	if (bobberBobTimer->IsFinished()) {
		tempBobberLoc = bobberLoc;
		bobberBobTimer->start(bobTime);
	}
}

AfishSchool* Acharacter::bobberInFishSchool() {
	if (!world::currWorld)
		return nullptr;

	for (std::unique_ptr<AfishSchool>& fishSchool : world::currWorld->fishSchoolList) {
		// check if bobber is inside of fish school
		// if it is then return true
		// otherwise keep looking
		if (fishSchool->pointInSchool(math::screenToWorld(bobberLoc)))
			return fishSchool.get();
	}
	return nullptr;
}

void Acharacter::calcFishSchool() {
	if (currFishSchool)
		currFishSchool->removeFishNum();

	currFishSchool = bobberInFishSchool();
}

bool Acharacter::canCatchWorldFish() {
	for (auto& fish : SaveData::data.fishData) {
		if (Scene::GetCurrWorldId() == fish.second.worldId) {
			if (Upgrades::Get(StatContext(Stat::Power)) < fish.second.fishingPower)
				return false;
			return true;
		}
	}
	return false;
}

void Acharacter::drawFishingLine(Shader* shaderProgram) {
	if (!isFishing || !showFishingLine)
		return;

	vector start = math::worldToScreen(fishingTipLoc).ToPixel();
	vector end = math::worldToScreen(bobberLoc).ToPixel();

	vector min = vector::min(start, end);
	vector max = vector::max(start, end);
	vector size = max - min;

	URectangle* rectangle = new URectangle(nullptr, min, size, false, glm::vec4(242.f / 255.f, 233.f / 255.f, 211.f / 255.f, 1.f));
	Scene::fishingLineShader->Activate();

	Scene::fishingLineShader->setVec2("start", glm::vec2(start.x, start.y));
	Scene::fishingLineShader->setVec2("end", glm::vec2(end.x, end.y));
	bool tight = anim->GetCurrAnim().find("wait") != std::string::npos || anim->GetCurrAnim().find("pull") != std::string::npos;
	Scene::fishingLineShader->setInt("tight", tight);

	rectangle->draw(Scene::fishingLineShader);
	delete rectangle;

	bobberImg->setLoc(bobberLoc);
	bobberWaterAnimBack->setLoc(tempBobberLoc);
	bobberWaterAnimFront->setLoc(tempBobberLoc);
	bobberWaterAnimBack->draw(shaderProgram);
	bobberImg->draw(shaderProgram);
	bobberWaterAnimFront->draw(shaderProgram);

	if (showFish)
		fishImg->draw(shaderProgram);
}

void Acharacter::setCatchPremium() {
	canCatchPremium = true;
}

void Acharacter::equipBait(uint32_t baitId) {
	uint32_t oldBaitId = SaveData::saveData.equippedBaitId;

	// make sure to change id first, so MarkDirty knows is unequipped
	SaveData::saveData.equippedBaitId = baitId;

	if (oldBaitId != 0) { // makes sure there was an previously equipped bait
		ModifierNode& oldBaitData = SaveData::data.modifierData.at(oldBaitId);
		Upgrades::MarkDirty(oldBaitData.stats);
	}

	ModifierNode& newBaitData = SaveData::data.modifierData.at(SaveData::saveData.equippedBaitId);
	Upgrades::MarkDirty(newBaitData.stats);
}

double Acharacter::GetCombo() {
	return comboNum;
}

bool Acharacter::IsFishingInSchool() {
	return currFishSchool != nullptr;
}

void Acharacter::IncreaseCombo(double comboChange) {
	comboNum = math::clamp(comboNum + comboChange, Upgrades::Get(Stat::ComboMin), Upgrades::Get(Stat::ComboMax));
}

void Acharacter::StartRecast(uint32_t fishId, double caughtNum) {
	recastActive = true;
	recastNum = 1;

	chainChance = Upgrades::Get(Stat::RecastChainChance);
	fishAtStartOfRecast = fishId;
	catchNumAtStartOfRecast = caughtNum;
	Recast();
}

void Acharacter::Recast() {
	double caught = catchNumAtStartOfRecast * recastNum;

	std::cout << "recasted!: " << recastNum << ", chainChance: " << chainChance << ", caught: " << caught << "\n";

	FsaveFishData& fishData = SaveData::saveData.fishData.at(fishAtStartOfRecast);
	fishData.unlocked = true;
	fishData.numOwned[currFishQuality] += caught;
	fishData.totalNumOwned[currFishQuality] += caught;

	numberWidget->Start(anim->getLoc() + anim->GetCellSize() / vector(2.f, 1.f), recastNum, NumberType::Recast);

	Main::heldFishWidget->updateList();

	// final fish num should be multiplied by recast num
		// so it would be like (fishNum * combo * upgrades * etc) * recastNum

	recastAudio->SetPitch(std::powf(2.f, recastNum / 12.f)); // increase pitch by 1 note
	recastAudio->Play();

	if (math::randRange(0.0, 100.0) <= chainChance) { // should continue chain
		chainChance *= Upgrades::Get(Stat::RecastFalloff); // reduce chance for next go
		recastTimer->start(0.25f); // arbitrary time between each recast effect
		recastNum++;
	} else
		recastActive = false;
}