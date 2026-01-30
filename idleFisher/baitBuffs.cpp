#include "baitBuffs.h"
#include "saveData.h"

/*
double baitBuffs::increaseFishSpeed() {
	uint32_t id = 1u; // pepper
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

bool baitBuffs::chanceToKeepCombo() {
	uint32_t id = 2u; // fishFood
	if (SaveData::saveData.equippedBaitId != id)
		return false;

	double rand = math::randRange(0.0, 1.0 / SaveData::data.baitData.at(id).buffValues[0]);
	return rand <= 1.0;
}

std::vector<double> baitBuffs::increaseFishCaughtIncraseFishInterval() {
	uint32_t id = 3u; // cannedMeat
	if (SaveData::saveData.equippedBaitId != id)
		return std::vector<double>{0, 0};
	return SaveData::data.baitData.at(id).buffValues;
}

std::vector<double> baitBuffs::decreaseFishIntervalDecraseFishCaught() {
	uint32_t id = 4u; // worm
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

double baitBuffs::increaseGoldenFishChance() {
	uint32_t id = 5u; // temp3
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

double baitBuffs::decreaseGoldenFishCooldown() {
	uint32_t id = 6u; // temp4
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

std::vector<double> baitBuffs::increaseYellowGreen() {
	uint32_t id = 7u; // temp5
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

double baitBuffs::decreaseFishMoveSpeed() {
	uint32_t id = 8u; // temp6
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

std::vector<double> baitBuffs::increaseGreenDecreaseYellow() {
	uint32_t id = 9u; // temp7
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

std::vector<double> baitBuffs::increaseYellowDecreaseGreen() {
	uint32_t id = 10u; // temp8
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

std::vector<double> baitBuffs::increaseFishSpeedIncreaseYellowGreen() {
	uint32_t id = 11u; // temp9
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

std::vector<double> baitBuffs::increaseFishSpeedCatchMoreFish() {
	uint32_t id = 12u; // temp10
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}

double baitBuffs::increaseFishComboBounce() {
	uint32_t id = 13u; // temp11
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

double baitBuffs::increaseComboMax() {
	uint32_t id = 14u; // temp12
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

double baitBuffs::increaseComboIncrease() {
	uint32_t id = 15u; // temp13
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

double baitBuffs::startAtHigherCombo() {
	uint32_t id = 16u; // temp14
	if (SaveData::saveData.equippedBaitId != id)
		return 0;
	return SaveData::data.baitData.at(id).buffValues[0];
}

std::vector<double> baitBuffs::increaseFishCaughtDecreaseFishIntervalsIncreaseCombo() {
	uint32_t id = 17u; // masterBait
	if (SaveData::saveData.equippedBaitId != id)
		return { 0, 0, 0 };
	return SaveData::data.baitData.at(id).buffValues;
}
*/