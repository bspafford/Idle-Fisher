#pragma once

#include <vector>

class petBuffs {
public:
	// i guess have a function for all pets, then the upgrade function will call the specific pet to see if its equipped?
	// if it is then do buff, else don't do buff

	// incrseases fish sell price by certain %, returns %, 0-1
	static double increaseFishPrice();
	// increases the max combo by certain %, returns %, 0-1
	static double increaseMaxCombo();
	// decreases fishing interval by certain %, returns %, 0-1
	static double decreaseFishingIntervals();
	// calcs if it should double fish, returns 1 || 2 depending on chance
	static double chanceToDoubleCatch();
	// increases bait buff value by certain %, returns %, 0-1
	static double increaseBaitBuff();
	// changes the values of fish probability by certain %, returns list of floats that then multiplies by probability of calcFishProbability function
	static std::vector<float> increaseChanceOfHigherFish();
	// increases golden fish buff by certain percent, returns %, 0-1
	static double increaseGoldenFishBuff();
	// reduces fish speed when fishing by certain %, return % 0-1
	// if value .25 then the speed will only be 75% of max
	static double decreaseFishComboSpeed();
	// 
	static double betterFishTransporter();
	// if value .25 then the speed will only be 75% of max
	static double increaseConverterSpeeds();
	// increases combo by certain %, returns %, 0-1
	static double increaseComboIncrease();
	static double increaseGoldenFishChance();
	// increases how fast the fish schools spawn by certain %, returns %, 0-1
	static float increaseFishSchoolSpawnRate();
	// reduces the time it takes for rain to spawn
	static float decreaseRainIntervals();
};