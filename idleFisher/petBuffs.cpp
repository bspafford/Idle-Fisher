#include "petBuffs.h"
#include "saveData.h"
#include "upgrades.h"

// should remove this class
// when equipping the pet, there should be a function that applies the buff to the relevant upgrade stat, then cache it
// so then i dont have to do this weird function/id check for every pet.

// was doing
/*
if (SaveData::saveData.equippedPetId != 658) // pig
		return 0;
return SaveData::data.petData.at(SaveData::saveData.equippedPetId).buffValue;
*/

double petBuffs::increaseFishPrice() {
	return 1;
}

double petBuffs::increaseMaxCombo() {
	return 1;
}

double petBuffs::decreaseFishingIntervals() {
	return 1;
}

double petBuffs::chanceToDoubleCatch() {
	return 1;
}

double petBuffs::increaseBaitBuff() {
	return 1;
}

std::vector<float> petBuffs::increaseChanceOfHigherFish() {
	return { 1, 1, 1, 1, 1 };
}

double petBuffs::increaseGoldenFishBuff() {
	return 1;
}

double petBuffs::decreaseFishComboSpeed() {
	return 1;
}

double petBuffs::betterFishTransporter() {
	return 1;
}

double petBuffs::increaseConverterSpeeds() {
	return 1;
}

double petBuffs::increaseComboIncrease() {
	return 1;
}

double petBuffs::increaseGoldenFishChance() {
	return 1;
}

float petBuffs::increaseFishSchoolSpawnRate() {
	return 1;
}

float petBuffs::decreaseRainIntervals() {
	return 1;
}