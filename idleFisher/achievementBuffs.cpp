#include "achievementBuffs.h"

#include "saveData.h"

void achievementBuffs::init() {
	// orderedmap sorted by ids to achievement func names
	for (auto& [key, value] : SaveData::data.achievementData)
		saveAchievementMap[value.name] = &SaveData::saveData.achievementList.at(key);
}

// a fucntion that finds the upgrade using a string
SaveEntry* achievementBuffs::getAchievement(std::string achievementFuncName) {
	return saveAchievementMap[achievementFuncName];
}

float achievementBuffs::getFishPercentIncrease(int fishId) {
	float percentIncrease = 1;
	SaveEntry* achievement = getAchievement("Catch your first fish!");
	if (!achievement)
		return 1;

	if (getAchievement("Catch your first fish!")->level)
		percentIncrease += .01f;
	if (getAchievement("catch 100 dirt fish")->level)
		percentIncrease += .01f;
	if (getAchievement("catch 10000 dirt fish")->level)
		percentIncrease += .01f;
	if (getAchievement("catch 1000000 dirt fish")->level)
		percentIncrease += .01f;

	return percentIncrease;
}