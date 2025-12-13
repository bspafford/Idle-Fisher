#include "baitBuffs.h"
#include "saveData.h"

double baitBuffs::increaseFishSpeed() {
	if (SaveData::saveData.equippedBait.id != 0)
		return 0;
	return SaveData::data.baitData[0].buffValues[0];
}

bool baitBuffs::chanceToKeepCombo() {
	if (SaveData::saveData.equippedBait.id != 1)
		return false;

	double rand = math::randRange(0.0, 1/ SaveData::data.baitData[1].buffValues[0]);
	return rand <= 1.0;
}

std::vector<double> baitBuffs::increaseFishCaughtIncraseFishInterval() {
	if (SaveData::saveData.equippedBait.id != 2)
		return std::vector<double>{0, 0};
	return SaveData::data.baitData[2].buffValues;
}

std::vector<double> baitBuffs::decreaseFishIntervalDecraseFishCaught() {
	if (SaveData::saveData.equippedBait.id != 3)
		return { 0, 0 };
	return SaveData::data.baitData[3].buffValues;
}

double baitBuffs::increaseGoldenFishChance() {
	if (SaveData::saveData.equippedBait.id != 4)
		return 0;
	return SaveData::data.baitData[4].buffValues[0];
}

double baitBuffs::decreaseGoldenFishCooldown() {
	if (SaveData::saveData.equippedBait.id != 5)
		return 0;
	return SaveData::data.baitData[5].buffValues[0];
}

std::vector<double> baitBuffs::increaseYellowGreen() {
	if (SaveData::saveData.equippedBait.id != 6)
		return { 0, 0 };
	return SaveData::data.baitData[6].buffValues;
}

double baitBuffs::decreaseFishMoveSpeed() {
	if (SaveData::saveData.equippedBait.id != 7)
		return 0;
	return SaveData::data.baitData[7].buffValues[0];
}

std::vector<double> baitBuffs::increaseGreenDecreaseYellow() {
	if (SaveData::saveData.equippedBait.id != 8)
		return { 0, 0 };
	return SaveData::data.baitData[8].buffValues;
}

std::vector<double> baitBuffs::increaseYellowDecreaseGreen() {
	if (SaveData::saveData.equippedBait.id != 9)
		return { 0, 0 };
	return SaveData::data.baitData[9].buffValues;
}

std::vector<double> baitBuffs::increaseFishSpeedIncreaseYellowGreen() {
	if (SaveData::saveData.equippedBait.id != 10)
		return { 0, 0, 0 };
	return SaveData::data.baitData[10].buffValues;
}

std::vector<double> baitBuffs::increaseFishSpeedCatchMoreFish() {
	if (SaveData::saveData.equippedBait.id != 11)
		return { 0, 0 };
	return SaveData::data.baitData[11].buffValues;
}

double baitBuffs::increaseFishComboBounce() {
	if (SaveData::saveData.equippedBait.id != 12)
		return 0;
	return SaveData::data.baitData[12].buffValues[0];
}

double baitBuffs::increaseComboMax() {
	if (SaveData::saveData.equippedBait.id != 13)
		return 0;
	return SaveData::data.baitData[13].buffValues[0];
}

double baitBuffs::increaseComboIncrease() {
	if (SaveData::saveData.equippedBait.id != 14)
		return 0;
	return SaveData::data.baitData[14].buffValues[0];
}

double baitBuffs::startAtHigherCombo() {
	if (SaveData::saveData.equippedBait.id != 15)
		return 0;
	return SaveData::data.baitData[15].buffValues[0];
}

std::vector<double> baitBuffs::increaseFishCaughtDecreaseFishIntervalsIncreaseCombo() {
	if (SaveData::saveData.equippedBait.id != 16)
		return { 0, 0, 0 };
	return SaveData::data.baitData[16].buffValues;
}