#pragma once

#include <unordered_map>
#include <string>

struct SaveEntry;

class achievementBuffs {
	static inline std::unordered_map<std::string, SaveEntry*> saveAchievementMap;
public:
	static void init();

	// a fucntion that finds the upgrade using a string
	static SaveEntry* getAchievement(std::string achievementFuncName);

	// claculates the fish % increase, based on players achievments
	// calculated on 
	// from fish in the worlds only
	// dirt fish achievement would improve leaf fish by 1% but wouldn't help bouy fish
	static float getFishPercentIncrease(int fishId);
};