#pragma once

#include <string>
#include <unordered_set>

#include "saveData.h"
#include "currencyWidget.h"
#include "upgradeBox.h"
#include "heldFishWidget.h"
#include "premiumBuffWidget.h"

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

	ShouldResetCombo = 20, // whether the combo should reset on a miss
	
// fishing rod
	Power = 11, // how strong the fishing rod is
	CatchNum = 12, // how many fish the player catches per cast

// Premium fish
	PremiumCatchChance = 13, // how common it is to catch the premium fish
	PremiumCoolDownTime = 14, // how long it takes until you can catch another premium fish
	PremiumBuff = 19, // calculates all active premium fish buffs

// fish school
	MaxFishSchoolSpawnInterval = 15,
	MinFishSchoolSpawnInterval = 16,

// rain
	MaxRainSpawnInterval = 17,
	MinRainSpawnInterval = 18,
};

enum class ModifierType {
	Buff = 1,
	Debuff = 2,
};

enum class ModifierActivation {
	Always = 1,
	Equipped = 2,
};

struct StatContext {
	Stat stat = Stat::None;
	uint32_t id = 0; // id of the context, fish id, bait id, pet id, upgrade id, vault unlock id

	double value = 0.0;

	// General Data
	StatContext(Stat _stat)
		: stat(_stat) {
	}

	// Green/Yellow Combo Size
	StatContext(Stat _stat, uint32_t _id)
		: stat(_stat), id(_id) {
	}

	// Combo Reset
	StatContext(Stat _stat, double _value)
		: stat(_stat), value(_value) {
	}

	// Fish Data, Fish Combo Speed
	StatContext(Stat _stat, uint32_t _id, double _value)
		: stat(_stat), id(_id), value(_value) {}
};

class Upgrades {
public:
	static void Init();

	// gets the final calculated stat value
	static double Get(const StatContext& statCtx);
	// gets the final calculated stat value
	static double Get(Stat stat);
	// gets the base stat value without any modifiers
	static double GetBaseStat(Stat s);

	// this function is called when an upgrade is purchased
	// increases the level of the upgrade
	// removes currency
	// and updates the cached value
	// returns true if successfully upgraded, price is outputted through reference, can be null
	static bool LevelUp(uint32_t upgradeId, int levels = 1);
	static double GetPrice(uint32_t upgradeId);
	static double GetPrice(const ProgressionNode& upgrade, SaveEntry& saveUpgrade, int levels = 1);

	static double GetCached(Stat stat);

	//static void AddModifier(Upgrade upgrade);
	//static void RemoveModifier(Upgrade upgrade);
	static void MarkDirty(Stat stat);
	static void MarkDirty(const std::unordered_map<Stat, ModData> stats);
	static void Update(double dt); // for temporary buffs

	// Updates all of the changed values at the end of the frame
	static void UpdateDirty();

private:
	static double Recalculate(Stat s);
	static bool IsModifierActive(const ModifierNode& modifier);

	// only cached the values for the next level, not multiple levels
	static inline std::unordered_map<uint32_t, double> cachedPrices;

	static inline std::unordered_map<Stat, std::unordered_set<uint32_t>> modifiersPerStat; // modifiers sorted by stat
	// the cached calculated upgrade values
	// things like fish price, fishing rod power, etc
	static inline std::unordered_map<Stat, double> cachedValues;
	static inline std::unordered_set<Stat> dirty;
};