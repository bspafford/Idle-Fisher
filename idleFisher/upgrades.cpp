#include "upgrades.h"
#include "main.h"
#include "character.h"
#include "petBuffs.h"
#include "baitBuffs.h"
#include "achievementBuffs.h"

void upgrades::init() {
	// converts list to unordered map
	for (int i = 0; i < SaveData::data.upgradeData.size(); i++) {
		saveUpgradeMap[SaveData::data.upgradeData[i].upgradeFunctionName] = &SaveData::saveData.upgradeList[i];
	}
}

// a fucntion that finds the upgrade using a string
FsaveUpgradeStruct* upgrades::getUpgrade(std::string upgradeFuncName) {
	return saveUpgradeMap[upgradeFuncName];
}

bool upgrades::upgrade(FupgradeStruct upgradeStruct, UupgradeBox* boxRef, double* price) {
	if (upgradeStruct.id == -1)
		return false;

	FsaveUpgradeStruct* saveUpgradeStruct = &SaveData::saveData.upgradeList[upgradeStruct.id];
	int worldId = worldNameToId(upgradeStruct.levelName);
	double upgradePrice = calcPrice(&upgradeStruct, saveUpgradeStruct);
	if (saveUpgradeStruct->upgradeLevel < upgradeStruct.upgradeNumMax && upgradePrice <= SaveData::saveData.currencyList[worldId + 1].numOwned) {
		SaveData::saveData.currencyList[worldId + 1].numOwned -= upgradePrice;
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
}

int upgrades::worldNameToId(std::string worldName) {
	for (FworldStruct worldData : SaveData::data.worldData) {
		if (worldData.worldName == worldName)
			return worldData.id;
	}
	return -1;
}

// calculates the price of the upgrade
double upgrades::calcPrice(FupgradeStruct* upgradeStruct, FsaveUpgradeStruct* saveUpgradeStruct) { // per upgrade
	std::string expression_string = upgradeStruct->priceEquation;
	double level = saveUpgradeStruct->upgradeLevel;
	exprtk::symbol_table<double> symbol_table;
	symbol_table.add_variable("level", level);
	exprtk::expression<double> expression;
	expression.register_symbol_table(symbol_table);
	exprtk::parser<double> parser;
	if (!parser.compile(expression_string, expression))
		std::cout << "parser error\n";
	double result = expression.value();
	return result;
}

// calculates what the value of the upgrade should be
double upgrades::upgradeEquation(FupgradeStruct* upgradeStruct, FsaveUpgradeStruct* saveUpgradeStruct) { // per upgrade
	std::string expression_string = upgradeStruct->upgradeNumEquation;
	double level = saveUpgradeStruct->upgradeLevel;
	exprtk::symbol_table<double> symbol_table;
	symbol_table.add_variable("level", level);
	exprtk::expression<double> expression;
	expression.register_symbol_table(symbol_table);
	exprtk::parser<double> parser;
	if (!parser.compile(expression_string, expression))
		std::cout << "parser error\n";
	double result = expression.value();
	return result;
}

double upgrades::calcFishingRodPowerPrice() {
	int level = SaveData::saveData.fishingRod.powerLevel;
	return 30 * pow(3, level);
}

double upgrades::calcFishingRodSpeedPrice() {
	int level = SaveData::saveData.fishingRod.speedLevel;
	return 10 * pow(2, level);
}

double upgrades::calcFishingRodCatchChancePrice() {
	int level = SaveData::saveData.fishingRod.catchChanceLevel;
	return 20 * pow(1.75, level);
}

// calculates how much the all the upgrades combine should equal
double upgrades::getFishSellPrice(FfishData fish, int quality) {

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
}

double upgrades::calcGreenFishingUpgrade() {
	FsaveUpgradeStruct* saveUpgrade = getUpgrade("increaseGreenComboSizeWorld6");
	double debuffVal = (1 - baitBuffs::increaseYellowDecreaseGreen()[1]);
	return debuffVal * (saveUpgrade->upgradeLevel + 1) * (baitBuffs::increaseYellowGreen()[1] + 1) * (baitBuffs::increaseGreenDecreaseYellow()[0] + 1) * (baitBuffs::increaseFishSpeedIncreaseYellowGreen()[2] + 1);
}

double upgrades::calcYellowFishingUpgrade() {
	FsaveUpgradeStruct* saveUpgrade = getUpgrade("increaseYellowComboSizeWorld6");
	double debuffVal = (1 - baitBuffs::increaseGreenDecreaseYellow()[1]);
	return debuffVal * (saveUpgrade->upgradeLevel + 1) * (baitBuffs::increaseYellowGreen()[0] + 1) * (baitBuffs::increaseYellowDecreaseGreen()[0] + 1) * (baitBuffs::increaseFishSpeedIncreaseYellowGreen()[1] + 1);
}

bool upgrades::IsComboUnlocked() {
	return getUpgrade("unlockComboWorld2")->upgradeLevel != 0;
}

double upgrades::calcComboMax() {
	FsaveUpgradeStruct* upgrade = getUpgrade("increaseComboMaxWorld2");
	return upgradeEquation(&SaveData::data.upgradeData[upgrade->id], upgrade) * (petBuffs::increaseMaxCombo() + 1) * (baitBuffs::increaseComboMax() + 1) * (baitBuffs::increaseFishCaughtDecreaseFishIntervalsIncreaseCombo()[2] + 1);
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
	return SaveData::saveData.fishingRod.powerLevel * 10;
}

double upgrades::calcFishingRodCatchChance() {
	return SaveData::saveData.fishingRod.catchChanceLevel;
}

double upgrades::calcFishCatchNum() {
	// calc golden fish buff
	double goldenFishVal = 1;
	for (UpremiumBuffWidget* premium : Main::premiumBuffList)
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
	return SaveData::saveData.fishingRod.powerLevel / 10;
}

int upgrades::calcFishingLineIndex() {
	return SaveData::saveData.fishingRod.speedLevel / 10;
}

int upgrades::calcBobberIndex() {
	return SaveData::saveData.fishingRod.catchChanceLevel / 10;
}