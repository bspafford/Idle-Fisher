#include "achievementBuffs.h"

#include "saveData.h"

void achievementBuffs::init() {
	// converts list to unordered map
	for (int i = 0; i < SaveData::data.achievementData.size(); i++) {
		saveAchievementMap[SaveData::data.achievementData[i].name] = &SaveData::saveData.achievementList[i];
	}
}

// a fucntion that finds the upgrade using a string
FsaveAchievementStruct* achievementBuffs::getAchievement(std::string achievementFuncName) {
	return saveAchievementMap[achievementFuncName];
}

float achievementBuffs::getFishPercentIncrease(int fishId) {
	float percentIncrease = 1;
	FsaveAchievementStruct* achievement = getAchievement("Catch your first fish!");
	if (!achievement)
		return 1;

	if (getAchievement("Catch your first fish!")->unlocked)
		percentIncrease += .01f;
	if (getAchievement("catch 100 dirt fish")->unlocked)
		percentIncrease += .01f;
	if (getAchievement("catch 10000 dirt fish")->unlocked)
		percentIncrease += .01f;
	if (getAchievement("catch 1000000 dirt fish")->unlocked)
		percentIncrease += .01f;

	return percentIncrease;
}