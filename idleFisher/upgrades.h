#pragma once

#include <string>
#include <unordered_set>

#include "saveData.h"
#include "currencyWidget.h"
#include "upgradeBox.h"
#include "heldFishWidget.h"
#include "premiumBuffWidget.h"
#include "exprtk.hpp"

enum class Stat {
// fish
	FishPrice, // how much fish sell for
	
// fish combo widget
	FishComboSpeed, // how fast the fish moves back and forth

	GreenComboSize, // how big green zone is
	YellowComboSize, // how big yellow zone is

	ComboUnlocked, // is combo unlocked
	ComboMax, // max combo
	ComboMin, // min combo

	ComboIncrease, // how much the combo increases when clicking green zone
	ComboDecreaseOnBounce, // how much the combo decreases when bouncing off the walls on a missed cycle
	ComboReset, // the min combo when clicking blue zone
	
// fishing rod
	Power, // how strong the fishing rod is
	CatchNum, // how many fish the player catches per cast

// Premium fish
	PremiumCatchChance, // how common it is to catch the premium fish
	PremiumCoolDownTime, // how long it takes until you can catch another premium fish

// fish school
	MaxFishSchoolSpawnInterval,
	MinFishSchoolSpawnInterval,

// rain
	MaxRainSpawnInterval,
	MinRainSpawnInterval,
};


struct Upgrade {
	Stat stat; // what this upgrade improves
	std::string id; // unique id for this upgrade, string for backwards compatibility
	int maxLevel = 1;
	// ((base + add * level) * multiply^level)^exponent
	double base = 0.0;
	double add = 0.0;
	double mul = 1.0;
	double exp = 1.0;
};

// for things like price
//struct Upgrade {
//	std::string name;
//	Stat targetStat; // what it affects
//	double basePrice;
//	double priceMultiplier; // per level: base * pow(priceMultiplier, level);
//	int& level;
//	int maxLevel = -1;
//};

class Stats {
public:
	static double Get(Stat s);
	static void AddModifier(Upgrade upgrade);
	static void RemoveModifier(Upgrade upgrade);
	static void MarkDirty(Stat s);
	static void Update(double dt); // for temporary buffs

	// Updates all of the changed values at the end of the frame
	static void UpdateDirty();

private:
	static inline std::unordered_map<std::string, Upgrade> allModifiers;
	static inline std::unordered_map<std::string, SaveEntry> saveModifiers;
	static inline std::unordered_map<Stat, std::vector<Upgrade*>> modifiersPerStat; // points to allModifiers modifier
	static inline std::unordered_map<Stat, double> cachedValues;
	static inline std::unordered_set<Stat> dirty;
};

class upgrades {
	static inline std::unordered_map<std::string, SaveEntry*> saveUpgradeMap;

public:
	static bool upgrade(FupgradeStruct upgradeStruct, UupgradeBox* boxRef, double* price = NULL);

	static void init() {}
	static double calcPrice(FupgradeStruct* upgradeStruct, SaveEntry* saveUpgradeStruct) { return 0; }
	static SaveEntry* getUpgrade(std::string upgradeFuncName);

	static double calcFishingRodPowerPrice();
	static double calcFishingRodSpeedPrice();
	static double calcFishingRodCatchChancePrice();

	// calculates how much the all the upgrades combine should equal
	static double getFishSellPrice(const FfishData& fish, int quality);

	static double calcGreenFishingUpgrade();
	static double calcYellowFishingUpgrade();

	static bool IsComboUnlocked();
	static double calcComboMax();
	static double calcComboMin(double comboMax);

	static double calcComboIncrease(double comboMax);
	// when the player clicks on the blue zone
	static double calcComboReset(double currCombo, double comboMax);
	

	// how many times the combo fish can go hit the walls until it goes away
	static double calcFishComboSpeed(FfishData currFish, int quality);
	static double calcComboDecreaseOnBounce();

	// fishing rod
	static double calcFishingRodPower();
	static double calcFishingRodCatchChance();

	static double calcFishCatchNum();

	static double calcConversionTime(FcurrencyConversionStruct* conversion);

	static double calcPremiumCatchChance();
	static float calcPremiumCoolDownTime();

	static float calcMaxFishSchoolSpawnInterval();
	static float calcMinFishSchoolSpawnInterval();

	static float calcMaxRainSpawnInterval();
	static float calcMinRainSpawnInterval();

	// returns the index of what current fishing rod the player is on
	static int calcFishingRodIndex();
	// returns the index of what current fishing line the player is on
	static int calcFishingLineIndex();
	// returns the index of what current bobber the player is on
	static int calcBobberIndex();
};