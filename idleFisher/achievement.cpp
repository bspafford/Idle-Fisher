#include "achievement.h"
#include "main.h"
#include "saveData.h"

void achievement::createAchievementList() {
	// fish number achievements
	achievements.push_back(std::make_unique<achievement>(1u,
		[]() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 1; }));
	achievements.push_back(std::make_unique<achievement>(2u,
		[]() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 100; }));
	achievements.push_back(std::make_unique<achievement>(3u,
			[]() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 10000; }));
	achievements.push_back(std::make_unique<achievement>(4u,
			[]() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 1000000; }));

	// money achievements
	achievements.push_back(std::make_unique<achievement>(5u,
			[]() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1; }));
	achievements.push_back(std::make_unique<achievement>(6u,
			[]() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 100; }));
	achievements.push_back(std::make_unique<achievement>(7u,
			[]() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 10000; }));
	achievements.push_back(std::make_unique<achievement>(8u,
			[]() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1000000; }));
}

void achievement::checkAchievements() {
	for (auto& achievement : achievements) {
		achievement->checkUnlock();
	}
}