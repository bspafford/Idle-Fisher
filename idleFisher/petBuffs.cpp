#include "petBuffs.h"
#include "saveData.h"
#include "main.h"

double petBuffs::increaseFishPrice() {
	if (SaveData::saveData.equippedPetId != 658) // pig
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;
}

double petBuffs::increaseMaxCombo() {
	if (SaveData::saveData.equippedPetId != 659) // monkey
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;
}

double petBuffs::decreaseFishingIntervals() {
	if (SaveData::saveData.equippedPetId != 660) // carb
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;
}

double petBuffs::chanceToDoubleCatch() {
	if (SaveData::saveData.equippedPetId != 661) // octopus
		return 1;

	int rand = math::randRange(0, 1 / SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue);
	if (rand <= 1)
		return 2;
	return 1;
}

double petBuffs::increaseBaitBuff() {
	if (SaveData::saveData.equippedPetId != 662) // worm
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

std::vector<float> petBuffs::increaseChanceOfHigherFish() {
	if (SaveData::saveData.equippedPetId != 663) // fox
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
	if (SaveData::saveData.equippedPetId != 664) // goldenFish
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

double petBuffs::decreaseFishComboSpeed() {
	if (SaveData::saveData.equippedPetId != 665) // trutle
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

double petBuffs::betterFishTransporter() {
	if (SaveData::saveData.equippedPetId != 666) // dog
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

double petBuffs::increaseConverterSpeeds() {
	if (SaveData::saveData.equippedPetId != 667) // rabbit
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

double petBuffs::increaseComboIncrease() {
	if (SaveData::saveData.equippedPetId != 668) // chase
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

double petBuffs::increaseGoldenFishChance() {
	if (SaveData::saveData.equippedPetId != 669) // chameleon
		return 0;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

float petBuffs::increaseFishSchoolSpawnRate() {
	if (SaveData::saveData.equippedPetId != 670) // hawkOne
		return 0.f;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}

float petBuffs::decreaseRainIntervals() {
	if (SaveData::saveData.equippedPetId != 671) // sadWhiteBush
		return 0.f;
	return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;

}