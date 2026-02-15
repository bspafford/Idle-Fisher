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
#include "achievement.h"

#include "debugger.h"

AfishTransporter::AfishTransporter(vector loc) : npc(loc) {
	std::cout << "i'm the fish transporter!" << std::endl;

	// find fish transporters id
	uint32_t worldId = Scene::GetCurrWorldId();
	for (uint32_t mechanicId : SaveData::orderedData.mechanicStruct) {
		ProgressionNode& data = SaveData::data.progressionData.at(mechanicId);
		if (data.worldId == worldId) {
			id = data.id;
			break;
		}
	}

	float duration = 0.13f;
	std::unordered_map<std::string, animDataStruct> animData;

	// idle
	animData.insert({ "idleNE", animDataStruct({0, 0 }, { 0, 0 }, true, duration) });
	animData.insert({ "idleNW", animDataStruct({1, 0 }, { 1, 0 }, true, duration) });
	animData.insert({ "idleE", animDataStruct({0, 1 }, { 0, 1 }, true, duration) });
	animData.insert({ "idleW", animDataStruct({1, 1 }, { 1, 1 }, true, duration) });
	animData.insert({ "idleN", animDataStruct({0, 2 }, { 0, 2 }, true, duration) });
	animData.insert({ "idleS", animDataStruct({1, 2 }, { 1, 2 }, true, duration) });
	animData.insert({ "idleSE", animDataStruct({0, 3 }, { 0, 3 }, true, duration) });
	animData.insert({ "idleSW", animDataStruct({1, 3 }, { 1, 3 }, true, duration) });

	// walking
	animData.insert({ "walkNE", animDataStruct({0, 8 }, { 5, 8 }, true, duration) });
	animData.insert({ "walkNW", animDataStruct({0, 9 }, { 5, 9 }, true, duration) });
	animData.insert({ "walkE", animDataStruct({0, 6 }, { 5, 6 }, true, duration) });
	animData.insert({ "walkW", animDataStruct({0, 7 }, { 5, 7 }, true, duration) });
	animData.insert({ "walkN", animDataStruct({0, 11 }, { 5, 11 }, true, duration) });
	animData.insert({ "walkS", animDataStruct({0, 10 }, { 5, 10 }, true, duration) });
	animData.insert({ "walkSE", animDataStruct({0, 4 }, { 5, 4 }, true, duration) });
	animData.insert({ "walkSW", animDataStruct({0, 5 }, { 5, 5 }, true, duration) });

	npcAnim = std::make_unique<animation>("npc/fishTransporter/fishTransporterSpriteSheet.png", 30, 42, animData, true, loc);
	npcAnim->SetUseAlpha(true);
	npcAnim->setAnimation("idleSE");
	npcAnim->start();
	npcAnim->addAnimEvent(2, this, &AfishTransporter::FootHitFloor);
	npcAnim->addAnimEvent(5, this, &AfishTransporter::FootHitFloor);

	std::unordered_map<std::string, animDataStruct> fishPileData;

	// idle // empty
	fishPileData.insert({ "emptyidleNE", animDataStruct({0, 0 }, { 0, 0 }, true, duration) });
	fishPileData.insert({ "emptyidleNW", animDataStruct({1, 0 }, { 1, 0 }, true, duration) });
	fishPileData.insert({ "emptyidleE", animDataStruct({0, 1 }, { 0, 1 }, true, duration) });
	fishPileData.insert({ "emptyidleW", animDataStruct({1, 1 }, { 1, 1 }, true, duration) });
	fishPileData.insert({ "emptyidleN", animDataStruct({0, 2 }, { 0, 2 }, true, duration) });
	fishPileData.insert({ "emptyidleS", animDataStruct({1, 2 }, { 1, 2 }, true, duration) });
	fishPileData.insert({ "emptyidleSE", animDataStruct({0, 3 }, { 0, 3 }, true, duration) });
	fishPileData.insert({ "emptyidleSW", animDataStruct({1, 3 }, { 1, 3 }, true, duration) });
	// slight
	fishPileData.insert({ "slightidleNE", animDataStruct({6, 0 }, { 6, 0 }, true, duration) });
	fishPileData.insert({ "slightidleNW", animDataStruct({7, 0 }, { 7, 0 }, true, duration) });
	fishPileData.insert({ "slightidleE", animDataStruct({6, 1 }, { 6, 1 }, true, duration) });
	fishPileData.insert({ "slightidleW", animDataStruct({7, 1 }, { 7, 1 }, true, duration) });
	fishPileData.insert({ "slightidleN", animDataStruct({6, 2 }, { 6, 2 }, true, duration) });
	fishPileData.insert({ "slightidleS", animDataStruct({7, 2 }, { 7, 2 }, true, duration) });
	fishPileData.insert({ "slightidleSE", animDataStruct({6, 3 }, { 6, 3 }, true, duration) });
	fishPileData.insert({ "slightidleSW", animDataStruct({7, 3 }, { 7, 3 }, true, duration) });
	// mid
	fishPileData.insert({ "mididleNE", animDataStruct({12, 0 }, { 12, 0 }, true, duration) });
	fishPileData.insert({ "mididleNW", animDataStruct({13, 0 }, { 13, 0 }, true, duration) });
	fishPileData.insert({ "mididleE", animDataStruct({12, 1 }, { 12, 1 }, true, duration) });
	fishPileData.insert({ "mididleW", animDataStruct({13, 1 }, { 13, 1 }, true, duration) });
	fishPileData.insert({ "mididleN", animDataStruct({12, 2 }, { 12, 2 }, true, duration) });
	fishPileData.insert({ "mididleS", animDataStruct({13, 2 }, { 13, 2 }, true, duration) });
	fishPileData.insert({ "mididleSE", animDataStruct({12, 3 }, { 12, 3 }, true, duration) });
	fishPileData.insert({ "mididleSW", animDataStruct({13, 3 }, { 13, 3 }, true, duration) });
	// full
	fishPileData.insert({ "fullidleNE", animDataStruct({18, 0 }, { 18, 0 }, true, duration) });
	fishPileData.insert({ "fullidleNW", animDataStruct({19, 0 }, { 19, 0 }, true, duration) });
	fishPileData.insert({ "fullidleE", animDataStruct({18, 1 }, { 18, 1 }, true, duration) });
	fishPileData.insert({ "fullidleW", animDataStruct({19, 1 }, { 19, 1 }, true, duration) });
	fishPileData.insert({ "fullidleN", animDataStruct({18, 2 }, { 18, 2 }, true, duration) });
	fishPileData.insert({ "fullidleS", animDataStruct({19, 2 }, { 19, 2 }, true, duration) });
	fishPileData.insert({ "fullidleSE", animDataStruct({18, 3 }, { 18, 3 }, true, duration) });
	fishPileData.insert({ "fullidleSW", animDataStruct({19, 3 }, { 19, 3 }, true, duration) });

	// walking // empty
	fishPileData.insert({ "emptywalkNE", animDataStruct({0, 8 }, { 5, 8 }, true, duration) });
	fishPileData.insert({ "emptywalkNW", animDataStruct({0, 9 }, { 5, 9 }, true, duration) });
	fishPileData.insert({ "emptywalkE", animDataStruct({0, 6 }, { 5, 6 }, true, duration) });
	fishPileData.insert({ "emptywalkW", animDataStruct({0, 7 }, { 5, 7 }, true, duration) });
	fishPileData.insert({ "emptywalkN", animDataStruct({0, 11 }, { 5, 11 }, true, duration) });
	fishPileData.insert({ "emptywalkS", animDataStruct({0, 10 }, { 5, 10 }, true, duration) });
	fishPileData.insert({ "emptywalkSE", animDataStruct({0, 4 }, { 5, 4 }, true, duration) });
	fishPileData.insert({ "emptywalkSW", animDataStruct({0, 5 }, { 5, 5 }, true, duration) });
	// slight
	fishPileData.insert({ "slightwalkNE", animDataStruct({6, 8 }, { 11, 8 }, true, duration) });
	fishPileData.insert({ "slightwalkNW", animDataStruct({6, 9 }, { 11, 9 }, true, duration) });
	fishPileData.insert({ "slightwalkE", animDataStruct({6, 6 }, { 11, 6 }, true, duration) });
	fishPileData.insert({ "slightwalkW", animDataStruct({6, 7 }, { 11, 7 }, true, duration) });
	fishPileData.insert({ "slightwalkN", animDataStruct({6, 11 }, { 11, 11 }, true, duration) });
	fishPileData.insert({ "slightwalkS", animDataStruct({6, 10 }, { 11, 10 }, true, duration) });
	fishPileData.insert({ "slightwalkSE", animDataStruct({6, 4 }, { 11, 4 }, true, duration) });
	fishPileData.insert({ "slightwalkSW", animDataStruct({6, 5 }, { 11, 5 }, true, duration) });
	// mid
	fishPileData.insert({ "midwalkNE", animDataStruct({12, 8 }, { 17, 8 }, true, duration) });
	fishPileData.insert({ "midwalkNW", animDataStruct({12, 9 }, { 17, 9 }, true, duration) });
	fishPileData.insert({ "midwalkE", animDataStruct({12, 6 }, { 17, 6 }, true, duration) });
	fishPileData.insert({ "midwalkW", animDataStruct({12, 7 }, { 17, 7 }, true, duration) });
	fishPileData.insert({ "midwalkN", animDataStruct({12, 11 }, { 17, 11 }, true, duration) });
	fishPileData.insert({ "midwalkS", animDataStruct({12, 10 }, { 17, 10 }, true, duration) });
	fishPileData.insert({ "midwalkSE", animDataStruct({12, 4 }, { 17, 4 }, true, duration) });
	fishPileData.insert({ "midwalkSW", animDataStruct({12, 5 }, { 17, 5 }, true, duration) });
	// full
	fishPileData.insert({ "fullwalkNE", animDataStruct({18, 8 }, { 23, 8 }, true, duration) });
	fishPileData.insert({ "fullwalkNW", animDataStruct({18, 9 }, { 23, 9 }, true, duration) });
	fishPileData.insert({ "fullwalkE", animDataStruct({18, 6 }, { 23, 6 }, true, duration) });
	fishPileData.insert({ "fullwalkW", animDataStruct({18, 7 }, { 23, 7 }, true, duration) });
	fishPileData.insert({ "fullwalkN", animDataStruct({18, 11 }, { 23, 11 }, true, duration) });
	fishPileData.insert({ "fullwalkS", animDataStruct({18, 10 }, { 23, 10 }, true, duration) });
	fishPileData.insert({ "fullwalkSE", animDataStruct({18, 4 }, { 23, 4 }, true, duration) });
	fishPileData.insert({ "fullwalkSW", animDataStruct({18, 5 }, { 23, 5 }, true, duration) });

	fishPileAnim = std::make_unique<animation>("npc/fishTransporter/fishPileSpriteSheet.png", 30, 42, fishPileData, true, loc);
	fishPileAnim->setAnimation("emptyidleSE");
	fishPileAnim->start();

	fullnessText = std::make_unique<text>(nullptr, "0/0", "straight", loc, true, false, TEXT_ALIGN_CENTER, false);

	collectTimer = CreateDeferred<Timer>();
	collectTimer->addCallback(this, &AfishTransporter::finishCollectTimer);
	collectTimer->addUpdateCallback(this, &AfishTransporter::collectTimerUpdate);

	progressBar = std::make_unique<UprogressBar>(nullptr, vector{ 25, 3 }, true);
	progressBar->setVisibility(false);

	walkSFX = std::make_unique<Audio>("grass1.wav", AudioType::SFX, loc);

	setLoc(loc);
	setupCollision();

	Astar = std::make_unique<AStar>();

	discovered = &discoveredFallback;

	SetStats();

	startPathFinding();
}

AfishTransporter::~AfishTransporter() {

}

void AfishTransporter::click() {
}

void AfishTransporter::setupCollision() {
}

void AfishTransporter::startPathFinding() {
	std::vector<std::unique_ptr<AautoFisher>>& autoFisherList = world::GetAutoFisherList();
	if (autoFisherList.size() != 0 && autoFisherIndex != -1 && (Astar->stopped || Astar->finished)) {
		Astar->startPathFinding(loc, autoFisherList[autoFisherIndex]->loc + vector{ 6, 21 });
	}
}

void AfishTransporter::draw(Shader* shaderProgram) {
	//Astar->drawBoard(shaderProgram);

	npcAnim->draw(shaderProgram);
	fishPileAnim->draw(shaderProgram);

	if (npcAnim->IsMouseOver(true) && fullnessText) {
		fullnessText->setLoc(loc + npcAnim->GetCellSize() / vector{ 2.f, 1.f });
		fullnessText->draw(shaderProgram);
		progressBar->setLoc(loc + vector{ -progressBar->getSize().x / 2 / stuff::pixelSize, npcAnim->GetCellSize().y + 11.f});
	} else {
		progressBar->setLoc(loc + vector{ -progressBar->getSize().x / 2 / stuff::pixelSize, npcAnim->GetCellSize().y + 3.f});
	}

	progressBar->draw(shaderProgram);
}

void AfishTransporter::update(float deltaTime) {
	if (canMove) {
		moveDir = vector(0, 0);
		if (Astar->followingPath) {
			double speed = Upgrades::Get(StatContext(Stat::FishTransporterSpeed, id));
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
		int y = round(1.f / 45.f * (angle + 45.f / 2.f)) + 3;
		if (y == -1)
			y = 7;

		if (npcAnim->GetCurrAnim() != walkAnimList[y]) {
			npcAnim->setAnimation(walkAnimList[y]);
			fishPileAnim->setAnimation(fullnessString + walkAnimList[y]);
		}
	} else {
		float angle = atan2(prevMove.y, prevMove.x) * 180 / M_PI;
		int y = round(1.f / 45.f * (angle + 45.f / 2.f)) + 3;
		if (npcAnim->GetCurrAnim() != idleAnimList[y]) {
			npcAnim->setAnimation(idleAnimList[y]);
			fishPileAnim->setAnimation(fullnessString + idleAnimList[y]);
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
	else // sell fish
		collectFish(NULL);

	canMove = true;
	autoFisherIndex++;
	progressBar->setVisibility(false);
	double maxCapacity = Upgrades::Get(StatContext(Stat::FishTransporterMaxCapacity, id));
	double capacity = CalcCapacity();
	if (autoFisherIndex >= world::currWorld->autoFisherList.size() || capacity >= maxCapacity) {
		autoFisherIndex = -1;
	}

	// check if fish transporter has enough space for even the smallest fish
	if (maxCapacity - capacity < FfishData::GetCheapestFishInWorld().basePrice)
		autoFisherIndex = -1;

	vector goTo = calcGoTo(autoFisherIndex);
	Astar->startPathFinding(loc, goTo); // move to next spot
}

void AfishTransporter::collectTimerUpdate() {
	progressBar->setPercent(collectTimer->getPercent());
}

float AfishTransporter::calcCollectTimer(AautoFisher* autoFisher, bool getMaxTime) {
	if (!getMaxTime)
		return calcCollectTimer(CalcCapacity(), autoFisher ? autoFisher->CalcCapacity() : 0.0, autoFisher != nullptr);
	return Upgrades::Get(StatContext(Stat::FishTransporterCollectSpeed, id)); // returns the full time no matter what
}

float AfishTransporter::calcCollectTimer(double held, double afHeld, bool collectingFromAF) {
	double maxCapacity = Upgrades::Get(StatContext(Stat::FishTransporterMaxCapacity, id));
	double val = 1.0;
	if (collectingFromAF)
		val = math::clamp((std::min(maxCapacity - held, afHeld) / maxCapacity), 0, 1);
	else // get what evers quicker, either the fish transporter fills up, or fish transporter empties the auto fisher
		val = math::clamp(held / maxCapacity, 0, 1);

	// max, takes 5s, 0 takes 0s
	return Upgrades::Get(StatContext(Stat::FishTransporterCollectSpeed, id)) * val;
}

void AfishTransporter::collectFish(AautoFisher* autoFisher) {
	double maxCapacity = Upgrades::Get(StatContext(Stat::FishTransporterMaxCapacity, id));
	if (autoFisher) {
		std::vector<uint32_t> afHeldFishId = sortFishList(autoFisher->heldFish);
		for (int i = 0; i < afHeldFishId.size(); i++) {
			FfishData* currFish = &SaveData::data.fishData.at(afHeldFishId[i]);
			FsaveFishData* saveCurrFish = &autoFisher->heldFish.at(afHeldFishId[i]);
			// max amount of fish the transporter can carry before full
			int fishMax = (maxCapacity - CalcCapacity()) / currFish->basePrice;
			if (saveCurrFish->numOwned[0] <= fishMax) { // if can hold all fish
				addFishtoHeld(saveCurrFish, saveCurrFish->numOwned[0]);
				saveCurrFish->numOwned[0] = 0;
			} else { // if too many fish to hold
				addFishtoHeld(saveCurrFish, fishMax);
				saveCurrFish->numOwned[0] -= fishMax;
			}
		}
		SaveData::saveData.autoFisherList.at(autoFisher->id).fullness = autoFisher->CalcCapacity(); // update fullness for save data
		autoFisher->startFishing();
		autoFisher->afMoreInfoUI->updateUI();
	} else { // sell fish
		// give player currency
		FsaveCurrencyStruct& currencyData = SaveData::saveData.currencyList.at(Scene::GetCurrWorldId());
		double currencyHeld = CalcCapacity() * Upgrades::GetBaseStat(Stat::FishPrice);
		currencyData.numOwned += currencyHeld;
		currencyData.totalNumOwned += currencyHeld;
		holding.clear();

		Achievements::CheckGroup(AchievementTrigger::CurrencyEarned);

		Main::currencyWidget->updateList();
		// update the ui so the upgrade button enables if have enough money
		for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
			world::currWorld->autoFisherList[i]->UI->updateUI();
	}

	// calc fullnessString
	double currency = CalcCapacity();
	float percent = currency / maxCapacity;
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

	fullnessText->setText(shortNumbers::convert2Short(currency) + "/" + shortNumbers::convert2Short(maxCapacity));
}

void AfishTransporter::addFishtoHeld(FsaveFishData* fish, double addNum) {
	auto it = holding.find(fish->id);
	if (it != holding.end()) {
		it->second.numOwned[0] += addNum;
	} else {
		FsaveFishData temp;
		temp.id = fish->id;
		temp.numOwned.resize(4);
		temp.numOwned[0] = addNum;
		holding.insert({ temp.id, temp });
	}
}

std::vector<uint32_t> AfishTransporter::sortFishList(const std::unordered_map<uint32_t, FsaveFishData>& map) {
	// map to vector
	std::vector<FsaveFishData> list;
	list.reserve(map.size());
	for (auto& [id, data] : map)
		list.push_back(data);

	std::vector<uint32_t> sortedList;
	int index = 0;
	double mostExpensive = 0;
	int size = list.size();
	for (int i = 0; i < size; i++) {
		index = -1;
		mostExpensive = -1;
		for (int j = 0; j < list.size(); j++) {
			double fishPrice = Upgrades::Get(StatContext(Stat::FishPrice, list[j].id, 0));
			if (mostExpensive < fishPrice) {
				mostExpensive = fishPrice;
				index = j;
			}
		}

		sortedList.push_back(list[index].id);
		list.erase(list.begin() + index);
	}

	return sortedList;
}

double AfishTransporter::CalcCapacity() {
	double capacity = 0.0;
	for (auto& [fishId, saveFishData] : holding) {
		FfishData& fishData = SaveData::data.fishData.at(fishId);
		capacity += saveFishData.numOwned[0] * fishData.basePrice;
	}
	return capacity;
}

void AfishTransporter::setLoc(vector loc) {
	this->loc = loc;
	if (npcAnim) {
		npcAnim->setLoc(loc - vector{ npcAnim->GetCellSize().x / 2.f, 0 });
		fishPileAnim->setLoc(npcAnim->getLoc());
	}
}

void AfishTransporter::SetStats() {
	SaveEntry* mechanicStruct = &SaveData::saveData.progressionData.at(Scene::GetCurrWorldId());

	double maxCapacity = Upgrades::Get(StatContext(Stat::FishTransporterMaxCapacity, id));
	fullnessText->setText(shortNumbers::convert2Short(CalcCapacity()) + "/" + shortNumbers::convert2Short(maxCapacity));
}

bool AfishTransporter::calcIfPlayerInfront() {
	if (npcAnim) {
		vector charLoc = GetCharacter()->getCharLoc();
		vector npcLoc = loc;
		return (charLoc.y < npcLoc.y);
	}
	return false;
}

uint32_t AfishTransporter::GetId() {
	return id;
}

std::unordered_map<uint32_t, FsaveFishData> AfishTransporter::FillWithRandomFish(double currency, bool fillHeldFish) {
	std::unordered_map<uint32_t, FsaveFishData> heldList;

	// loop throguh all auto fishers in world
	// add up all their probabilities / total
	std::unordered_map<uint32_t, float> probabilities;
	float totalProb = 0.f;
	for (auto& autoFisher : world::currWorld->autoFisherList) {
		std::vector<std::pair<uint32_t, float>> probability = autoFisher->calcFishProbability(SaveData::data.fishData, false);
		for (auto& [id, prob] : probability) {
			probabilities[id] += prob;
			totalProb += prob;
		}
	}

	// calculate fish
	for (auto& [id, prob] : probabilities) {
		double fishPrice = Upgrades::Get(StatContext(Stat::FishPrice, id));
		float percent = prob / totalProb;
		double fishNum = percent * (currency / fishPrice);

		if (fishNum < 1.0)
			continue;

		FsaveFishData saveFishData;
		saveFishData.id = id;
		saveFishData.numOwned[0] = fishNum;

		heldList.insert({ id, saveFishData });
	}

	if (fillHeldFish)
		holding = heldList;
	return heldList;
}

void AfishTransporter::FootHitFloor() {
	if (npcAnim->GetCurrAnim().starts_with("walk")) {

		vector loc = npcAnim->getLoc() + vector(npcAnim->GetCellSize().x / 2.f, 0.f);

		std::string audioPath = "dirt1.wav";
		for (Fcollision* col : collision::GetGroundCollision()) {
			if (collision::IsPointInsidePolygon(col, loc)) {
				float rand = math::randRange(0.f, 1.f) < 0.5f;
				if (col->identifier == 'g') // grass
					audioPath = rand ? "grass1.wav" : "grass2.wav";
				else if (col->identifier == 'o') // wood
					audioPath = rand ? "wood1.wav" : "wood2.wav";
				else if (col->identifier == 'm') // metal
					audioPath = rand ? "metal1.wav" : "metal2.wav";
				else // dirt
					audioPath = rand ? "dirt1.wav" : "dirt2.wav";
				break;
			}
		}

		walkSFX->SetLoc(loc);
		walkSFX->SetSpeed(math::randRange(0.9f, 1.1f));
		walkSFX->SetAudio(audioPath);
		walkSFX->Play();
	}
}