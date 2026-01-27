#include "upgrades.h"
#include "main.h"
#include "character.h"
#include "petBuffs.h"
#include "baitBuffs.h"
#include "achievementBuffs.h"

double Stats::Get(Stat s) {
	return cachedValues.at(s);
}

void Stats::AddModifier(Upgrade upgrade) {
	// what do i want to pass add modifier?
	// maybe just the upgradeId and a reference to the SaveData value?

	// then add it to allModifiers if it isn't already
	// maybe the modifier could be a reference to the SaveData level?

	// the finally mark dirty


	// so then in the list i will have the upgradeId and the level that will auto update? 
	// but i need to make sure everytime the level updates the mark that stat dirty
	// 

	auto [it, inserted] = allModifiers.emplace(upgrade.id, upgrade);
	if (inserted) // only add modifier if it wasn't already in allModifiers list
		modifiersPerStat[upgrade.stat].push_back(&it->second);

	MarkDirty(upgrade.stat);
}

void Stats::RemoveModifier(Upgrade upgrade) {

}

void Stats::MarkDirty(Stat s) {
	dirty.insert(s);
}

void Stats::Update(double dt) {

}

void Stats::UpdateDirty() {
	for (auto& stat : dirty) { // loop through dirty stats
		double& cachedValue = cachedValues[stat];
		for (Upgrade* mod : modifiersPerStat[stat]) { // recalculate all modifiers of stat
			SaveEntry& saveMod = saveModifiers.at(mod->id);
			cachedValue += std::pow((mod->base + mod->add * saveMod.level) * std::pow(mod->mul, saveMod.level), mod->exp);
		}
	}

	dirty.clear();
}










SaveEntry* upgrades::getUpgrade(std::string upgradeFuncName) {
	return saveUpgradeMap[upgradeFuncName];
}

bool upgrades::upgrade(FupgradeStruct upgradeStruct, UupgradeBox* boxRef, double* price) {
	return false;
	/*
	if (upgradeStruct.id == "")
		return false;

	SaveEntry* saveUpgradeStruct = &SaveData::saveData.upgradeList[upgradeStruct.id];
	double upgradePrice = calcPrice(&upgradeStruct, saveUpgradeStruct);
	if (saveUpgradeStruct->level < upgradeStruct.upgradeNumMax && upgradePrice <= SaveData::saveData.currencyList.at(upgradeStruct.levelName).numOwned) {
		SaveData::saveData.currencyList.at(upgradeStruct.levelName).numOwned -= upgradePrice;
		saveUpgradeStruct->upgradeLevel++;
		saveUpgradeStruct->value = upgradeEquation(&upgradeStruct, saveUpgradeStruct);
		saveUpgradeStruct->price = calcPrice(&upgradeStruct, saveUpgradeStruct); // calc new price
		if (price)
			*price = saveUpgradeStruct->price;
		Main::currencyWidget->updateList();
		Main::heldFishWidget->updateList();
		boxRef->update();
		return true;
	}
	return false;
	*/
}

double upgrades::calcFishingRodPowerPrice() {
	int level = SaveData::saveData.fishingRod.power.level;
	return 30 * pow(3, level);
}

double upgrades::calcFishingRodSpeedPrice() {
	int level = SaveData::saveData.fishingRod.speed.level;
	return 10 * pow(2, level);
}

double upgrades::calcFishingRodCatchChancePrice() {
	int level = SaveData::saveData.fishingRod.catchChance.level;
	return 20 * pow(1.75, level);
}

// calculates how much the all the upgrades combine should equal
double upgrades::getFishSellPrice(const FfishData& fish, int quality) {
	return 1;
	/*
	// upgrades
	double value = 0;
	value += getUpgrade("fishSellPriceWorld1")->value;
	value += getUpgrade("fishSellPriceWorld2")->value;
	value += getUpgrade("fishSellPriceWorld3")->value;
	value += getUpgrade("fishSellPriceWorld4")->value;
	value += getUpgrade("fishSellPriceWorld5")->value;
	value += getUpgrade("fishSellPriceWorld6")->value;
	value += getUpgrade("fishSellPriceWorld7")->value;
	value += getUpgrade("fishSellPriceWorld8")->value;
	value += getUpgrade("fishSellPriceWorld9")->value;
	value += getUpgrade("fishSellPriceWorld10")->value;

	// pet stuff
	double petBuff = petBuffs::increaseFishPrice();

	return fish.currencyNum * (value + 1) * (petBuff + 1) * achievementBuffs::getFishPercentIncrease(fish.id) * (1.f + (quality * .1f));
	*/
}

double upgrades::calcGreenFishingUpgrade() {
	return 1;
	/*
	SaveEntry* saveUpgrade = getUpgrade("increaseGreenComboSizeWorld6");
	double debuffVal = (1 - baitBuffs::increaseYellowDecreaseGreen()[1]);
	return debuffVal * (saveUpgrade->level + 1) * (baitBuffs::increaseYellowGreen()[1] + 1) * (baitBuffs::increaseGreenDecreaseYellow()[0] + 1) * (baitBuffs::increaseFishSpeedIncreaseYellowGreen()[2] + 1);
	*/
}

double upgrades::calcYellowFishingUpgrade() {
	return 1;
	/*
	SaveEntry* saveUpgrade = getUpgrade("increaseYellowComboSizeWorld6");
	double debuffVal = (1 - baitBuffs::increaseGreenDecreaseYellow()[1]);
	return debuffVal * (saveUpgrade->level + 1) * (baitBuffs::increaseYellowGreen()[0] + 1) * (baitBuffs::increaseYellowDecreaseGreen()[0] + 1) * (baitBuffs::increaseFishSpeedIncreaseYellowGreen()[1] + 1);
	*/
}

bool upgrades::IsComboUnlocked() {
	return false;
	//return getUpgrade("unlockComboWorld2")->upgradeLevel != 0;
}

double upgrades::calcComboMax() {
	return 1;
	/*
	SaveEntry* upgrade = getUpgrade("increaseComboMaxWorld2");
	return upgradeEquation(&SaveData::data.upgradeData[upgrade->id], upgrade) * (petBuffs::increaseMaxCombo() + 1) * (baitBuffs::increaseComboMax() + 1) * (baitBuffs::increaseFishCaughtDecreaseFishIntervalsIncreaseCombo()[2] + 1);
	*/
}

double upgrades::calcComboMin(double comboMax) {
	return math::clamp(comboMax * baitBuffs::startAtHigherCombo(), 1.0, comboMax);
}

double upgrades::calcComboIncrease(double comboMax) {
	return math::clamp(comboMax * (petBuffs::increaseComboIncrease() + baitBuffs::increaseComboIncrease()), 1.0, comboMax);
}

double upgrades::calcComboReset(double currCombo, double comboMax) {
	return math::max(currCombo / 2.f, calcComboMin(comboMax));
}

double upgrades::calcFishComboSpeed(FfishData currFish, int quality) {
	// look at fishing rod
	// look at fish speed
	double speed = (0.1 * GetCharacter()->GetCombo() + 0.9) / (upgrades::calcFishingRodPower() / 5.0) * static_cast<double>(currFish.fishSpeed);
	speed *= (baitBuffs::increaseFishSpeed() + 1.0) * (baitBuffs::increaseFishSpeedIncreaseYellowGreen()[0] + 1.0) * (baitBuffs::increaseFishSpeedCatchMoreFish()[0] + 1.0); // debuff
	speed *= (1.0 - petBuffs::decreaseFishComboSpeed()) * (1.0 - baitBuffs::decreaseFishMoveSpeed()) * (1.0 + (0.1 * quality)); // buff

	return math::max(speed, 0.1); // makes minimum speed 0.1 so fish will still move if too strong
}

double upgrades::calcComboDecreaseOnBounce() {
	return 1;
}

double upgrades::calcFishingRodPower() {
	return SaveData::saveData.fishingRod.power.level * 10;
}

double upgrades::calcFishingRodCatchChance() {
	return SaveData::saveData.fishingRod.catchChance.level;
}

double upgrades::calcFishCatchNum() {
	// calc golden fish buff
	double goldenFishVal = 1;
	for (auto& premium : Main::premiumBuffList)
		goldenFishVal *= premium->getGoldenFish().multiplier * (petBuffs::increaseGoldenFishBuff() + 1);

	double debuffVal = (1 - baitBuffs::decreaseFishIntervalDecraseFishCaught()[1]);
	double buffVal = GetCharacter()->GetCombo() * GetCharacter()->getFishSchoolMultiplier() * goldenFishVal * petBuffs::chanceToDoubleCatch() * (baitBuffs::increaseFishCaughtIncraseFishInterval()[0] + 1) * (baitBuffs::increaseFishSpeedCatchMoreFish()[1] + 1) * (baitBuffs::increaseFishCaughtDecreaseFishIntervalsIncreaseCombo()[0] + 1);
	return round(debuffVal * buffVal);
}

double upgrades::calcConversionTime(FcurrencyConversionStruct* conversion) {
	return conversion->timerMax - (conversion->timerMax * petBuffs::increaseConverterSpeeds());
}

double upgrades::calcPremiumCatchChance() {
	return SaveData::data.fishData[0].probability * (petBuffs::increaseGoldenFishChance() + 1) * (baitBuffs::increaseGoldenFishChance() + 1);
}

float upgrades::calcPremiumCoolDownTime() {
	double baseVal = 900; // 15 min
	return static_cast<float>(baseVal * (1 - baitBuffs::decreaseGoldenFishCooldown()));
}

float upgrades::calcMaxFishSchoolSpawnInterval() {
	float baseVal = 120;
	return baseVal - (baseVal * petBuffs::increaseFishSchoolSpawnRate());
}

float upgrades::calcMinFishSchoolSpawnInterval() {
	float baseVal = 60;
	return baseVal - (baseVal * petBuffs::increaseFishSchoolSpawnRate());
}

float upgrades::calcMaxRainSpawnInterval() {
	float baseVal = 60 * 60; // 60 min
	return baseVal - (baseVal * petBuffs::decreaseRainIntervals());
}

float upgrades::calcMinRainSpawnInterval() {
	float baseVal = 30 * 60; // 30 min
	return baseVal - (baseVal * petBuffs::decreaseRainIntervals());
}

int upgrades::calcFishingRodIndex() {
	return SaveData::saveData.fishingRod.power.level / 10;
}

int upgrades::calcFishingLineIndex() {
	return SaveData::saveData.fishingRod.speed.level / 10;
}

int upgrades::calcBobberIndex() {
	return SaveData::saveData.fishingRod.catchChance.level / 10;
}