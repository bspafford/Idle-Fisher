#include "fishTransporter.h"
#include "main.h"
#include "character.h"
#include "worlds.h"
#include "AautoFisher.h"
#include "upgrades.h"
#include "timer.h"
#include "progressBar.h"
#include "currencyWidget.h"
#include "autoFisherUI.h"
#include "AFmoreInfoUI.h"
#include "animation.h"
#include "idleProfitWidget.h"
#include "text.h"
#include "shortNumbers.h"

#include "debugger.h"

AfishTransporter::AfishTransporter(vector loc) : npc(loc) {
	std::cout << "i'm the fish transporter!" << std::endl;

	// walk
	std::string walkAnimPath = "./images/npc/fishTransporter/";
	walkE = std::vector<std::string>{ walkAnimPath + "walkE1.png", walkAnimPath + "walkE2.png", walkAnimPath + "walkE3.png", walkAnimPath + "walkE4.png", walkAnimPath + "walkE5.png", walkAnimPath + "walkE6.png" };
	walkNE = std::vector<std::string>{ walkAnimPath + "walkNE1.png", walkAnimPath + "walkNE2.png", walkAnimPath + "walkNE3.png", walkAnimPath + "walkNE4.png", walkAnimPath + "walkNE5.png", walkAnimPath + "walkNE6.png" };
	walkN = std::vector<std::string>{ walkAnimPath + "walkN1.png", walkAnimPath + "walkN2.png", walkAnimPath + "walkN3.png", walkAnimPath + "walkN4.png", walkAnimPath + "walkN5.png", walkAnimPath + "walkN6.png" };
	walkNW = std::vector<std::string>{ walkAnimPath + "walkNW1.png", walkAnimPath + "walkNW2.png", walkAnimPath + "walkNW3.png", walkAnimPath + "walkNW4.png", walkAnimPath + "walkNW5.png", walkAnimPath + "walkNW6.png" };
	walkW = std::vector<std::string>{ walkAnimPath + "walkW1.png", walkAnimPath + "walkW2.png", walkAnimPath + "walkW3.png", walkAnimPath + "walkW4.png", walkAnimPath + "walkW5.png", walkAnimPath + "walkW6.png" };
	walkSW = std::vector<std::string>{ walkAnimPath + "walkSW1.png", walkAnimPath + "walkSW2.png", walkAnimPath + "walkSW3.png", walkAnimPath + "walkSW4.png", walkAnimPath + "walkSW5.png", walkAnimPath + "walkSW6.png" };
	walkS = std::vector<std::string>{ walkAnimPath + "walkS1.png", walkAnimPath + "walkS2.png", walkAnimPath + "walkS3.png", walkAnimPath + "walkS4.png", walkAnimPath + "walkS5.png", walkAnimPath + "walkS6.png" };
	walkSE = std::vector<std::string>{ walkAnimPath + "walkSE1.png", walkAnimPath + "walkSE2.png", walkAnimPath + "walkSE3.png", walkAnimPath + "walkSE4.png", walkAnimPath + "walkSE5.png", walkAnimPath + "walkSE6.png" };
	walkAnimWheel = std::vector<std::vector<std::string>>{ walkNW, walkN, walkNE, walkE, walkSE, walkS, walkSW, walkW };

	// idle
	std::string idleAnimPath = "./images/character/animation/idle/";
	idleE = std::vector<std::string>{ idleAnimPath + "idleE.png" };
	idleNE = std::vector<std::string>{ idleAnimPath + "idleNE.png" };
	idleN = std::vector<std::string>{ idleAnimPath + "idleN.png" };
	idleNW = std::vector<std::string>{ idleAnimPath + "idleNW.png" };
	idleW = std::vector<std::string>{ idleAnimPath + "idleW.png" };
	idleSW = std::vector<std::string>{ idleAnimPath + "idleSW.png" };
	idleS = std::vector<std::string>{ idleAnimPath + "idleS.png" };
	idleSE = std::vector<std::string>{ idleAnimPath + "idleSE.png" };
	idleAnimWheel = std::vector<std::vector<std::string>>{ idleNW, idleN, idleNE, idleE, idleSE, idleS, idleSW, idleW };

	std::unordered_map<std::string, animDataStruct> animData;
	// idle
	animData.insert({ "idleNE", { { 0, 0 }, { 0, 0 }, .13, true } });
	animData.insert({ "idleNW", { { 1, 0 }, { 1, 0 }, .13, true } });
	animData.insert({ "idleE", { { 0, 1 }, { 0, 1 }, .13, true } });
	animData.insert({ "idleW", { { 1, 1 }, { 1, 1 }, .13, true } });
	animData.insert({ "idleN", { { 0, 2 }, { 0, 2 }, .13, true } });
	animData.insert({ "idleS", { { 1, 2 }, { 1, 2 }, .13, true } });
	animData.insert({ "idleSE", { { 0, 3 }, { 0, 3 }, .13, true } });
	animData.insert({ "idleSW", { { 1, 3 }, { 1, 3 }, .13, true } });

	// walking
	animData.insert({ "walkNE", { { 0, 8 }, { 5, 8 }, .13, true } });
	animData.insert({ "walkNW", { { 0, 9 }, { 5, 9 }, .13, true } });
	animData.insert({ "walkE", { { 0, 6 }, { 5, 6 }, .13, true } });
	animData.insert({ "walkW", { { 0, 7 }, { 5, 7 }, .13, true } });
	animData.insert({ "walkN", { { 0, 11 }, { 5, 11 }, .13, true } });
	animData.insert({ "walkS", { { 0, 10 }, { 5, 10 }, .13, true } });
	animData.insert({ "walkSE", { { 0, 4 }, { 5, 4 }, .13, true } });
	animData.insert({ "walkSW", { { 0, 5 }, { 5, 5 }, .13, true } });

	npcAnim = std::make_unique<animation>("npc/fishTransporter/fishTransporterSpriteSheet.png", 30, 42, animData, true, loc);
	npcAnim->spriteSheet->setUseAlpha(true);
	npcAnim->setAnimation("idleSE");
	npcAnim->start();

	std::unordered_map<std::string, animDataStruct> fishPileData;
	// idle // empty
	fishPileData.insert({ "emptyidleNE", { { 0, 0 }, { 0, 0 }, .13, true } });
	fishPileData.insert({ "emptyidleNW", { { 1, 0 }, { 1, 0 }, .13, true } });
	fishPileData.insert({ "emptyidleE", { { 0, 1 }, { 0, 1 }, .13, true } });
	fishPileData.insert({ "emptyidleW", { { 1, 1 }, { 1, 1 }, .13, true } });
	fishPileData.insert({ "emptyidleN", { { 0, 2 }, { 0, 2 }, .13, true } });
	fishPileData.insert({ "emptyidleS", { { 1, 2 }, { 1, 2 }, .13, true } });
	fishPileData.insert({ "emptyidleSE", { { 0, 3 }, { 0, 3 }, .13, true } });
	fishPileData.insert({ "emptyidleSW", { { 1, 3 }, { 1, 3 }, .13, true } });
	// slight
	fishPileData.insert({ "slightidleNE", { { 6, 0 }, { 6, 0 }, .13, true } });
	fishPileData.insert({ "slightidleNW", { { 7, 0 }, { 7, 0 }, .13, true } });
	fishPileData.insert({ "slightidleE", { { 6, 1 }, { 6, 1 }, .13, true } });
	fishPileData.insert({ "slightidleW", { { 7, 1 }, { 7, 1 }, .13, true } });
	fishPileData.insert({ "slightidleN", { { 6, 2 }, { 6, 2 }, .13, true } });
	fishPileData.insert({ "slightidleS", { { 7, 2 }, { 7, 2 }, .13, true } });
	fishPileData.insert({ "slightidleSE", { { 6, 3 }, { 6, 3 }, .13, true } });
	fishPileData.insert({ "slightidleSW", { { 7, 3 }, { 7, 3 }, .13, true } });
	// mid
	fishPileData.insert({ "mididleNE", { { 12, 0 }, { 12, 0 }, .13, true } });
	fishPileData.insert({ "mididleNW", { { 13, 0 }, { 13, 0 }, .13, true } });
	fishPileData.insert({ "mididleE", { { 12, 1 }, { 12, 1 }, .13, true } });
	fishPileData.insert({ "mididleW", { { 13, 1 }, { 13, 1 }, .13, true } });
	fishPileData.insert({ "mididleN", { { 12, 2 }, { 12, 2 }, .13, true } });
	fishPileData.insert({ "mididleS", { { 13, 2 }, { 13, 2 }, .13, true } });
	fishPileData.insert({ "mididleSE", { { 12, 3 }, { 12, 3 }, .13, true } });
	fishPileData.insert({ "mididleSW", { { 13, 3 }, { 13, 3 }, .13, true } });
	// full
	fishPileData.insert({ "fullidleNE", { { 18, 0 }, { 18, 0 }, .13, true } });
	fishPileData.insert({ "fullidleNW", { { 19, 0 }, { 19, 0 }, .13, true } });
	fishPileData.insert({ "fullidleE", { { 18, 1 }, { 18, 1 }, .13, true } });
	fishPileData.insert({ "fullidleW", { { 19, 1 }, { 19, 1 }, .13, true } });
	fishPileData.insert({ "fullidleN", { { 18, 2 }, { 18, 2 }, .13, true } });
	fishPileData.insert({ "fullidleS", { { 19, 2 }, { 19, 2 }, .13, true } });
	fishPileData.insert({ "fullidleSE", { { 18, 3 }, { 18, 3 }, .13, true } });
	fishPileData.insert({ "fullidleSW", { { 19, 3 }, { 19, 3 }, .13, true } });

	// walking // empty
	fishPileData.insert({ "emptywalkNE", { { 0, 8 }, { 5, 8 }, .13, true } });
	fishPileData.insert({ "emptywalkNW", { { 0, 9 }, { 5, 9 }, .13, true } });
	fishPileData.insert({ "emptywalkE", { { 0, 6 }, { 5, 6 }, .13, true } });
	fishPileData.insert({ "emptywalkW", { { 0, 7 }, { 5, 7 }, .13, true } });
	fishPileData.insert({ "emptywalkN", { { 0, 11 }, { 5, 11 }, .13, true } });
	fishPileData.insert({ "emptywalkS", { { 0, 10 }, { 5, 10 }, .13, true } });
	fishPileData.insert({ "emptywalkSE", { { 0, 4 }, { 5, 4 }, .13, true } });
	fishPileData.insert({ "emptywalkSW", { { 0, 5 }, { 5, 5 }, .13, true } });
	// slight
	fishPileData.insert({ "slightwalkNE", { { 6, 8 }, { 11, 8 }, .13, true } });
	fishPileData.insert({ "slightwalkNW", { { 6, 9 }, { 11, 9 }, .13, true } });
	fishPileData.insert({ "slightwalkE", { { 6, 6 }, { 11, 6 }, .13, true } });
	fishPileData.insert({ "slightwalkW", { { 6, 7 }, { 11, 7 }, .13, true } });
	fishPileData.insert({ "slightwalkN", { { 6, 11 }, { 11, 11 }, .13, true } });
	fishPileData.insert({ "slightwalkS", { { 6, 10 }, { 11, 10 }, .13, true } });
	fishPileData.insert({ "slightwalkSE", { { 6, 4 }, { 11, 4 }, .13, true } });
	fishPileData.insert({ "slightwalkSW", { { 6, 5 }, { 11, 5 }, .13, true } });
	// mid
	fishPileData.insert({ "midwalkNE", { { 12, 8 }, { 17, 8 }, .13, true } });
	fishPileData.insert({ "midwalkNW", { { 12, 9 }, { 17, 9 }, .13, true } });
	fishPileData.insert({ "midwalkE", { { 12, 6 }, { 17, 6 }, .13, true } });
	fishPileData.insert({ "midwalkW", { { 12, 7 }, { 17, 7 }, .13, true } });
	fishPileData.insert({ "midwalkN", { { 12, 11 }, { 17, 11 }, .13, true } });
	fishPileData.insert({ "midwalkS", { { 12, 10 }, { 17, 10 }, .13, true } });
	fishPileData.insert({ "midwalkSE", { { 12, 4 }, { 17, 4 }, .13, true } });
	fishPileData.insert({ "midwalkSW", { { 12, 5 }, { 17, 5 }, .13, true } });
	// full
	fishPileData.insert({ "fullwalkNE", { { 18, 8 }, { 23, 8 }, .13, true } });
	fishPileData.insert({ "fullwalkNW", { { 18, 9 }, { 23, 9 }, .13, true } });
	fishPileData.insert({ "fullwalkE", { { 18, 6 }, { 23, 6 }, .13, true } });
	fishPileData.insert({ "fullwalkW", { { 18, 7 }, { 23, 7 }, .13, true } });
	fishPileData.insert({ "fullwalkN", { { 18, 11 }, { 23, 11 }, .13, true } });
	fishPileData.insert({ "fullwalkS", { { 18, 10 }, { 23, 10 }, .13, true } });
	fishPileData.insert({ "fullwalkSE", { { 18, 4 }, { 23, 4 }, .13, true } });
	fishPileData.insert({ "fullwalkSW", { { 18, 5 }, { 23, 5 }, .13, true } });

	fishPileAnim = std::make_unique<animation>("npc/fishTransporter/fishPileSpriteSheet.png", 30, 42, fishPileData, true, loc);
	fishPileAnim->setAnimation("emptyidleSE");
	fishPileAnim->start();

	fullnessText = std::make_unique<text>(nullptr, "0/" + shortNumbers::convert2Short(maxHoldNum), "straight", loc, true, false, TEXT_ALIGN_CENTER);

	collectTimer = std::make_unique<timer>();
	collectTimer->addCallback(this, &AfishTransporter::finishCollectTimer);
	collectTimer->addUpdateCallback(this, &AfishTransporter::collectTimerUpdate);

	progressBar = std::make_unique<UprogressBar>(nullptr, true, 25, 3);
	progressBar->setVisibility(false);

	setLoc(loc);
	setupCollision();

	Astar = std::make_unique<AStar>();
	startPathFinding();

	discovered = &discoveredFallback;
}

AfishTransporter::~AfishTransporter() {

}

void AfishTransporter::click() {
}

void AfishTransporter::setupCollision() {
}

void AfishTransporter::startPathFinding() {
	if (world::currWorld->autoFisherList.size() != 0 && autoFisherIndex != -1 && (Astar->stopped || Astar->finished)) {
		Astar->startPathFinding(loc, world::currWorld->autoFisherList[autoFisherIndex]->loc + vector{ 6, 21 });
	}
}

void AfishTransporter::draw(Shader* shaderProgram) {
	//Astar->drawBoard(shaderProgram);

	npcAnim->draw(shaderProgram);
	fishPileAnim->draw(shaderProgram);

	if (npcAnim->spriteSheet->isMouseOver(true) && fullnessText) {
		fullnessText->setLoc(loc + vector(npcAnim->cellWidth / 2.f, npcAnim->cellHeight));
		fullnessText->draw(shaderProgram);
		progressBar->setLoc(loc + vector{ -progressBar->getSize().x / 2 / stuff::pixelSize, npcAnim->cellHeight + 11.f });
	} else {
		progressBar->setLoc(loc + vector{ -progressBar->getSize().x / 2 / stuff::pixelSize, npcAnim->cellHeight + 3.f });
	}

	progressBar->draw(shaderProgram);
}

void AfishTransporter::update(float deltaTime) {
	if (canMove) {
		if (Astar->followingPath) {
			vector diff = Astar->followPath(loc, deltaTime, speed);
			//std::cout << ", diff: " << math::length(diff) << std::endl;
			moveDir = math::normalize(diff);
			setLoc(loc + diff * speed);
		} else if (Astar->finished && canMove) {
			canMove = false;
			if (autoFisherIndex != -1)
				collectTimer->start(calcCollectTimer(world::currWorld->autoFisherList[autoFisherIndex].get()));
			else
				collectTimer->start(calcCollectTimer(NULL));
			progressBar->setVisibility(true);
		}
	}

	setAnimation();
}

void AfishTransporter::setAnimation() {
	// animate character depending on move direction
	std::vector<std::string> walkAnimList = { "walkSW", "walkS", "walkSE", "walkE", "walkNE", "walkN", "walkNW", "walkW" };
	std::vector<std::string> idleAnimList = { "idleSW", "idleS", "idleSE", "idleE", "idleNE", "idleN", "idleNW", "idleW" };

	if (moveDir.x != 0 || moveDir.y != 0) {
		prevMove = moveDir;
		float angle = atan2(moveDir.y, moveDir.x) * 180 / M_PI;
		int y = floor(1.f / 45.f * (angle + 45.f / 2.f)) + 3;
		if (y == -1)
			y = 7;

		if (npcAnim->currAnim != walkAnimList[y]) {
			npcAnim->setAnimation(walkAnimList[y], true);
			fishPileAnim->setAnimation(fullnessString + walkAnimList[y], true);
		}
	} else {
		float angle = atan2(prevMove.y, prevMove.x) * 180 / M_PI;
		int y = floor(1.f / 45.f * (angle + 45.f / 2.f)) + 3;
		if (npcAnim->currAnim != idleAnimList[y]) {
			npcAnim->setAnimation(idleAnimList[y], true);
			fishPileAnim->setAnimation(fullnessString + idleAnimList[y], true);
		}
	}
}

vector AfishTransporter::calcGoTo(int autoFisherIndex) {
	if (autoFisherIndex != -1)
		return world::currWorld->autoFisherList[autoFisherIndex]->loc + vector{ 6, 21 };
	return world::currWorld->bankSellLoc;
}

void AfishTransporter::finishCollectTimer() {

	// take fish from autofisher
	if (autoFisherIndex != -1)
		collectFish(world::currWorld->autoFisherList[autoFisherIndex].get());
	else
		collectFish(NULL);

	canMove = true;
	autoFisherIndex++;
	progressBar->setVisibility(false);
	if (autoFisherIndex >= world::currWorld->autoFisherList.size() || calcCurrencyHeld() >= maxHoldNum) {
		autoFisherIndex = -1;
	}

	vector goTo = calcGoTo(autoFisherIndex);
	Astar->startPathFinding(loc, goTo); // move to next spot
}

void AfishTransporter::collectTimerUpdate() {
	progressBar->setPercent(collectTimer->getTime() / collectTimer->getMaxTime());
}

float AfishTransporter::calcCollectTimer(AautoFisher* autoFisher, bool getMaxTime) {
	double val = 1;
	if (!getMaxTime)
		val = math::clamp(autoFisher ? (autoFisher->calcCurrencyHeld() / maxHoldNum) : (calcCurrencyHeld() / maxHoldNum), 0, 1);

	// max, takes 5s, 0 takes 0s
	return collectionSpeed * val;
}

void AfishTransporter::collectFish(AautoFisher* autoFisher) {
	if (autoFisher) {
		sortFishList(autoFisher->heldFish);
		for (int i = 0; i < autoFisher->heldFish.size(); i++) {
			FsaveFishData* saveCurrFish = &autoFisher->heldFish[i];
			FfishData* currFish = &SaveData::data.fishData[saveCurrFish->id];

			// max amount of fish the transporter can carry before full
			int fishMax = (maxHoldNum - calcCurrencyHeld()) / currFish->currencyNum;
			if (saveCurrFish->numOwned[0] <= fishMax) { // if can hold all fish
				addFishtoHeld(saveCurrFish, saveCurrFish->numOwned[0]);
				saveCurrFish->numOwned[0] = 0;
			} else { // if too many fish to hold
				addFishtoHeld(saveCurrFish, fishMax);
				saveCurrFish->numOwned[0] -= fishMax;
			}
		}
		autoFisher->startFishing();
		autoFisher->afMoreInfoUI->updateUI();
	} else { // sell fish
		// give player currency
		SaveData::saveData.currencyList[1].numOwned += calcCurrencyHeld();
		SaveData::saveData.currencyList[1].totalNumOwned += calcCurrencyHeld();
		holding.clear();
		Main::currencyWidget->updateList();
		// update the ui so the upgrade button enables if have enough money
		for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
			world::currWorld->autoFisherList[i]->UI->updateUI();
	}

	// calc fullnessString
	double currency = calcCurrencyHeld();
	float percent = currency / maxHoldNum;
	// 0 - 25
	// 25 - 50
	// 50 - 75
	// 75 - 100
	if (percent <= .25)
		fullnessString = "empty";
	else if (percent <= .50)
		fullnessString = "slight";
	else if (percent <= .75)
		fullnessString = "mid";
	else
		fullnessString = "full";

	fullnessText->setText(shortNumbers::convert2Short(currency) + "/" + shortNumbers::convert2Short(maxHoldNum));
}

void AfishTransporter::addFishtoHeld(FsaveFishData* fish, double addNum) {
	bool added = false;
	for (int i = 0; i < holding.size(); i++) {
		if (holding[i].id == fish->id) {
			added = true;
			holding[i].numOwned[0] += addNum;
		}
	}
	if (!added) {
		FsaveFishData temp;
		temp.id = fish->id;
		temp.numOwned.push_back(addNum);
		holding.push_back(temp);
	}
}

void AfishTransporter::sortFishList(std::vector<FsaveFishData> &list) {
	std::vector<FsaveFishData> sortedList;
	int index = 0;
	int biggestId = -1;
	int size = list.size();
	for (int i = 0; i < size; i++) {
		index = -1;
		biggestId = -1;
		for (int j = 0; j < list.size(); j++) {
			if (biggestId < list[j].id) {
				biggestId = list[j].id;
				index = j;
			}
		}

		sortedList.push_back(list[index]);
		list.erase(list.begin() + index);
	}

	list = sortedList;
}

double AfishTransporter::calcCurrencyHeld() {
	double currency = 0;
	for (int i = 0; i < holding.size(); i++) {
		FsaveFishData saveFish = holding[i];
		FfishData fish = SaveData::data.fishData[saveFish.id];
		currency += upgrades::getFishSellPrice(fish, 0) * saveFish.numOwned[0];
	}

	return currency;
}

void AfishTransporter::setLoc(vector loc) {
	this->loc = loc;
	if (npcAnim) {
		npcAnim->setLoc(loc - vector{ npcAnim->cellWidth / 2.f, 0 });
		fishPileAnim->setLoc(npcAnim->getLoc());
	}
}

// this method doesn't calculate the path finding distance, it just uses straight lines
void AfishTransporter::calcIdleProfits(float timeDiff) {
	if (world::currWorld->autoFisherList.size() == 0)
		return;

	double totalCurrency = 0;
	for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
		if (world::currWorld->autoFisherList[i])
			totalCurrency += world::currWorld->autoFisherList[i]->calcIdleProfits(timeDiff);

	// collection time per loop is maxCollectTime * autoFisher num
	float maxCollectTime = calcCollectTimer(NULL, true) * world::currWorld->autoFisherList.size(); // if the autofishers aren't max this is wasting time

	// then calculate how long it takes to walk to each autofisher including selling
	vector start = calcGoTo(-1); // gets start loc
	float totalDist = math::distance(start, world::currWorld->autoFisherList[0]->loc); // distances from start to 1
	for (int i = 1; i < world::currWorld->autoFisherList.size() - 1; i++) {
		// loop through
		// get distance from 1 to 2, 2 to 3...
		totalDist += math::distance(world::currWorld->autoFisherList[i]->loc, world::currWorld->autoFisherList[i + 1]->loc);
	}
	// distance from last to start
	totalDist += math::distance(world::currWorld->autoFisherList[world::currWorld->autoFisherList.size() - 1]->loc, start);

	float walkTime = totalDist / speed;
	// then see how long the loops take, go until you use all the afk timer

	float loopTime = maxCollectTime + walkTime; // how long it takes for the fish transporter to do 1 full loop

	float loopNum = timeDiff / loopTime;
	double currency = 0;
	for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
		currency += world::currWorld->autoFisherList[i]->calcMPS() * loopTime * loopNum;

	double currencyMade = 0; // temp
	if (currency > totalCurrency) { // then collect totalCurrency
		SaveData::saveData.currencyList[1].numOwned += totalCurrency; // temp
		SaveData::saveData.currencyList[1].totalNumOwned += totalCurrency; // temp
		currencyMade = totalCurrency; // temp


	} else { // collect currency, and calc how much the autofishers should contain
		SaveData::saveData.currencyList[1].numOwned += currency;
		SaveData::saveData.currencyList[1].totalNumOwned += currency;
		currencyMade = currency; // temp

		double remainingCurrency = totalCurrency - currency;

		for (int i = world::currWorld->autoFisherList.size() - 1; i >= 0; i--) {
			AautoFisher* currAutoFisher = world::currWorld->autoFisherList[i].get();
			if (remainingCurrency > currAutoFisher->maxCurrency - currAutoFisher->calcCurrencyHeld()) {
				// add fish to autofisher
				// temp
				if (currAutoFisher->heldFish.size() > 0)
					currAutoFisher->heldFish[0].numOwned[0] = currAutoFisher->maxCurrency; // temp
				else {
					FsaveFishData fish;
					fish.id = 1;
					fish.numOwned[0] = currAutoFisher->maxCurrency;
					currAutoFisher->heldFish.push_back(fish);
				}
				std::cout << "heldfish size: " << currAutoFisher->heldFish.size() << std::endl;
				// remove from remaining currency
				remainingCurrency -= currAutoFisher->maxCurrency;
			} else {
				// add to autofisher
				currAutoFisher->heldFish[0].numOwned[0] += remainingCurrency;
				break;
			}
		}
	}

	Main::currencyWidget->updateList();

	// temp
	FsaveFishData fish;
	fish.id = 1;
	fish.numOwned[0] = currencyMade;
	std::vector<FsaveFishData> temp = std::vector<FsaveFishData>{fish};

	Main::idleProfitWidget->setup(temp);

	std::cout << "loopNum: " << std::endl;
	std::cout << "distance: " << totalDist << std::endl;
	std::cout << "walk time: " << walkTime << std::endl;
	std::cout << "totalCurrency: " << totalCurrency << std::endl;
	std::cout << "currency: " << currency << std::endl;
}

void AfishTransporter::upgrade(FsaveMechanicStruct* mechanicStruct) {
	mechanicStruct->level++;

	maxHoldNum = mechanicStruct->level * 100;
	//speed = 2.0202 * mechanicStruct->level + 47.9798;
	//collectionSpeed = -0.040404 * mechanicStruct->level + 5.0404; // level 1 = 5 sec, level 100 = 1 sec

	// every 10 levels, upgrade speed
	if (mechanicStruct->level % 10 == 0) {
		speed = 2.0202 * mechanicStruct->level + 47.9798;
		collectionSpeed = -0.040404 * mechanicStruct->level + 5.0404; // level 1 = 5 sec, level 100 = 1 sec
	}

	fullnessText->setText(shortNumbers::convert2Short(calcCurrencyHeld()) + "/" + shortNumbers::convert2Short(maxHoldNum));

}

bool AfishTransporter::calcIfPlayerInfront() {
	if (npcAnim) {
		vector charLoc = Acharacter::getCharLoc();
		vector npcLoc = loc;
		return (charLoc.y < npcLoc.y);
	}
	return false;
}

double AfishTransporter::getMaxHoldNum() {
	return maxHoldNum;
}

float AfishTransporter::getSpeed() {
	return speed;
}

float AfishTransporter::getCollectionSpeed() {
	return collectionSpeed;
}