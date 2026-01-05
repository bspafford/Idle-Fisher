#pragma once

#include <string>
#include "saveData.h"
#include "currencyWidget.h"
#include "upgradeBox.h"
#include "heldFishWidget.h"
#include "premiumBuffWidget.h"
#include "exprtk.hpp"

class upgrades {
	static inline std::unordered_map<std::string, FsaveUpgradeStruct*> saveUpgradeMap;
public:
	static void init();

	// a fucntion that finds the upgrade using a string
	static FsaveUpgradeStruct* getUpgrade(std::string upgradeFuncName);

	static bool upgrade(FupgradeStruct upgradeStruct, UupgradeBox* boxRef, double* price = NULL);

	static int worldNameToId(std::string worldName);

	// calculates the price of the upgrade
	static double calcPrice(FupgradeStruct* upgradeStruct, FsaveUpgradeStruct* saveUpgradeStruct);

	// calculates what the value of the upgrade should be
	static double upgradeEquation(FupgradeStruct* upgradeStruct, FsaveUpgradeStruct* saveUpgradeStruct);

	static double calcFishingRodPowerPrice();
	static double calcFishingRodSpeedPrice();
	static double calcFishingRodCatchChancePrice();

	// calculates how much the all the upgrades combine should equal
	static double getFishSellPrice(FfishData fish, int quality);

	static double calcGreenFishingUpgrade();
	static double calcYellowFishingUpgrade();

	static bool IsComboUnlocked();
	static double calcComboMax();
	static double calcComboIncrease(double comboMax);
	static double calcComboStart(double comboMax);

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

	

	// decrease fishing interval
	// fish transporter
	// Vault
	// unlock currency converter
	// less input currency converter
	// more output currency converter
	// more input more output
	// increase yellow
	// increase green
	// make combo go up more per fish
	// catch more fish per cast for idle fishers
	// 


	// should prolly be mechanic or something else
	// increase idle fisher speed
	// increase fish carrier capacity
};