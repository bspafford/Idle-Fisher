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
	None = 0, // no stat

// fish
	FishPrice = 1, // how much fish sell for
	
// fish combo widget
	FishComboSpeed = 2, // how fast the fish moves back and forth

	GreenComboSize = 3, // how big green zone is
	YellowComboSize = 4, // how big yellow zone is

	ComboUnlocked = 5, // is combo unlocked
	ComboMax = 6, // max combo
	ComboMin = 7, // min combo

	ComboIncrease = 8, // how much the combo increases when clicking green zone
	ComboDecreaseOnBounce = 9, // how much the combo decreases when bouncing off the walls on a missed cycle
	ComboReset = 10, // the min combo when clicking blue zone
	
// fishing rod
	Power = 11, // how strong the fishing rod is
	CatchNum = 12, // how many fish the player catches per cast

// Premium fish
	PremiumCatchChance = 13, // how common it is to catch the premium fish
	PremiumCoolDownTime = 14, // how long it takes until you can catch another premium fish

// fish school
	MaxFishSchoolSpawnInterval = 15,
	MinFishSchoolSpawnInterval = 16,

// rain
	MaxRainSpawnInterval = 17,
	MinRainSpawnInterval = 18,
};

enum class StatContextType {
	None = 0,
	Fish = 1,
	Bait = 2,
	Pet = 3,
	Upgrade = 4,
	VaultUnlock = 5,
};

struct StatContext {
	Stat stat = Stat::None;
	uint32_t id = 0; // id of the context, fish id, bait id, pet id, upgrade id, vault unlock id

	double value = 0.0;

	// Fish Data
	StatContext(Stat _stat, uint32_t _id, double _value)
		: stat(_stat), id(_id), value(_value) {}
};

class Upgrades {
public:
	static void Init();

	// gets the final calculated stat value
	static double Get(const StatContext& statCtx);

	// this function is called when an upgrade is purchased
	// increases the level of the upgrade
	// removes currency
	// and updates the cached value
	// returns true if successfully upgraded, price is outputted through reference, can be null
	static bool LevelUp(uint32_t upgradeId, Stat stat, int levels = 1);
	static double GetPrice(uint32_t upgradeId);
	static double GetPrice(const ProgressionNode& upgrade, SaveEntry& saveUpgrade, int levels = 1);

	static double GetCached(Stat stat);

	//static void AddModifier(Upgrade upgrade);
	//static void RemoveModifier(Upgrade upgrade);
	static void MarkDirty(Stat s);
	static void Update(double dt); // for temporary buffs

	// Updates all of the changed values at the end of the frame
	static void UpdateDirty();

private:
	// gets the base stat value without any modifiers/base values
	static double GetStat(Stat s);
	static double Recalculate(Stat s);

	// only cached the values for the next level, not multiple levels
	static inline std::unordered_map<uint32_t, double> cachedPrices;

	static inline std::unordered_map<Stat, std::unordered_set<uint32_t>> modifiersPerStat; // modifiers sorted by stat
	// the cached calculated upgrade values
	// things like fish price, fishing rod power, etc
	static inline std::unordered_map<Stat, double> cachedValues;
	static inline std::unordered_set<Stat> dirty;
};

class upgrades {
	static inline std::unordered_map<std::string, SaveEntry*> saveUpgradeMap;

public:
	static bool upgrade(ModifierNode upgradeStruct, UupgradeBox* boxRef, double* price = NULL);

	static void init();
	static double calcPrice(ModifierNode* upgradeStruct, SaveEntry* saveUpgradeStruct) { return 0; }
	static SaveEntry* getUpgrade(std::string upgradeFuncName);

	static double calcGreenFishingUpgrade();
	static double calcYellowFishingUpgrade();

	static bool IsComboUnlocked();
	static double calcComboMax();
	static double calcComboMin(double comboMax);

	static double calcComboIncrease(double comboMax);
	// when the player clicks on the blue zone
	static double calcComboReset(double currCombo, double comboMax);
	

	// how many times the combo fish can go hit the walls until it goes away
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