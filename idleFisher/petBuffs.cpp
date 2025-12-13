#include "petBuffs.h"
#include "saveData.h"
#include "main.h"

double petBuffs::increaseFishPrice() {
	if (SaveData::saveData.equippedPet.id != 0)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;
}

double petBuffs::increaseMaxCombo() {
	if (SaveData::saveData.equippedPet.id != 1)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;
}

double petBuffs::decreaseFishingIntervals() {
	if (SaveData::saveData.equippedPet.id != 2)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;
}

double petBuffs::chanceToDoubleCatch() {
	if (SaveData::saveData.equippedPet.id != 3)
		return 1;

	int rand = math::randRange(0, 1/SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue);
	if (rand <= 1)
		return 2;
	return 1;
}

double petBuffs::increaseBaitBuff() {
	if (SaveData::saveData.equippedPet.id != 4)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

std::vector<float> petBuffs::increaseChanceOfHigherFish() {
	if (SaveData::saveData.equippedPet.id != 5)
		return std::vector<float>(0);
	std::vector<float> temp;
	temp.push_back(1);		// unchanged
	temp.push_back(1);		// unchanged
	temp.push_back(1.25);	// * 1.25 prob
	temp.push_back(1.5);	// * 1.5 prob
	temp.push_back(2);		// last fish * 2 prob
	return temp;
}

double petBuffs::increaseGoldenFishBuff() {
	if (SaveData::saveData.equippedPet.id != 6)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

double petBuffs::decreaseFishComboSpeed() {
	if (SaveData::saveData.equippedPet.id != 7)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

double petBuffs::betterFishTransporter() {
	if (SaveData::saveData.equippedPet.id != 8)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

double petBuffs::increaseConverterSpeeds() {
	if (SaveData::saveData.equippedPet.id != 9)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

double petBuffs::increaseComboIncrease() {
	if (SaveData::saveData.equippedPet.id != 10)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

double petBuffs::increaseGoldenFishChance() {
	if (SaveData::saveData.equippedPet.id != 11)
		return 0;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

float petBuffs::increaseFishSchoolSpawnRate() {
	if (SaveData::saveData.equippedPet.id != 12)
		return 0.f;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}

float petBuffs::decreaseRainIntervals() {
	if (SaveData::saveData.equippedPet.id != 13)
		return 0.f;
	return SaveData::data.petData[SaveData::saveData.equippedPet.id].buffValue;

}