#include "achievement.h"
#include "main.h"
#include "saveData.h"
#include "upgrades.h"

void Achievements::Init() {
	AddAchievement(117u, []() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 1; }); // catch your first fish

	// clicks
	AddAchievement(118u, []() { return SaveData::saveData.clicks >= 1000; }); // reel in 1,000 fish (click 1,000 times
	AddAchievement(119u, []() { return SaveData::saveData.clicks >= 10000; }); // reel in 10,000 fish
	AddAchievement(120u, []() { return SaveData::saveData.clicks >= 100000; }); // reel in 100,000 fish
	AddAchievement(121u, []() { return SaveData::saveData.clicks >= 1000000; }); // reel in 1 Million fish

	// fish number achievements
	// dirt fish
	AddAchievement(122u, []() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 100; }); // catch 100 dirt fish
	AddAchievement(123u, []() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 10000; }); // catch 10,000 dirt fish
	AddAchievement(124u, []() { return SaveData::saveData.fishData.at(2u).calcTotalCaughtFish() >= 1000000; }); // catch 1 million dirt fish
	// leaf fish
	AddAchievement(125u, []() { return SaveData::saveData.fishData.at(3u).calcTotalCaughtFish() >= 1; }); // catch a leaf fish
	AddAchievement(126u, []() { return SaveData::saveData.fishData.at(3u).calcTotalCaughtFish() >= 200; }); // catch 200 leaf fish
	AddAchievement(127u, []() { return SaveData::saveData.fishData.at(3u).calcTotalCaughtFish() >= 20000; }); // catch 20,000 leaf fish
	AddAchievement(128u, []() { return SaveData::saveData.fishData.at(3u).calcTotalCaughtFish() >= 2000000; }); // catch 2 million leaf fish
	// rock fish
	AddAchievement(129u, []() { return SaveData::saveData.fishData.at(4u).calcTotalCaughtFish() >= 1; }); // catch a rock fish
	AddAchievement(130u, []() { return SaveData::saveData.fishData.at(4u).calcTotalCaughtFish() >= 300; }); // catch 300 rock fish
	AddAchievement(131u, []() { return SaveData::saveData.fishData.at(4u).calcTotalCaughtFish() >= 30000; }); // catch 30,000 rock fish
	AddAchievement(132u, []() { return SaveData::saveData.fishData.at(4u).calcTotalCaughtFish() >= 3000000; }); // catch 3 million rock fish
	// stick fish
	AddAchievement(133u, []() { return SaveData::saveData.fishData.at(5u).calcTotalCaughtFish() >= 1; }); // catch a stick fish
	AddAchievement(134u, []() { return SaveData::saveData.fishData.at(5u).calcTotalCaughtFish() >= 400; }); // catch 400 stick fish
	AddAchievement(135u, []() { return SaveData::saveData.fishData.at(5u).calcTotalCaughtFish() >= 40000; }); // catch 40,000 stick fish
	AddAchievement(136u, []() { return SaveData::saveData.fishData.at(5u).calcTotalCaughtFish() >= 4000000; }); // catch 4 million stick fish
	// tree fish
	AddAchievement(137u, []() { return SaveData::saveData.fishData.at(6u).calcTotalCaughtFish() >= 1; }); // catch a tree fish
	AddAchievement(138u, []() { return SaveData::saveData.fishData.at(6u).calcTotalCaughtFish() >= 500; }); // catch 500 tree fish
	AddAchievement(139u, []() { return SaveData::saveData.fishData.at(6u).calcTotalCaughtFish() >= 50000; }); // catch 50,000 tree fish
	AddAchievement(140u, []() { return SaveData::saveData.fishData.at(6u).calcTotalCaughtFish() >= 5000000; }); // catch 5 million tree fish

	// special fish
	AddAchievement(141u, []() { return SaveData::saveData.fishData.at(7u).calcTotalCaughtFish() >= 1; }); // special fish 1
	AddAchievement(142u, []() { return SaveData::saveData.fishData.at(8u).calcTotalCaughtFish() >= 1; }); // special fish 2
	AddAchievement(143u, []() { return SaveData::saveData.fishData.at(9u).calcTotalCaughtFish() >= 1; }); // special fish 3
	AddAchievement(144u, []() { return CaughtSpecialFish(1); }); // catch a special fish
	AddAchievement(145u, []() { return CaughtSpecialFish(100); }); // catch 100 special fish
	AddAchievement(146u, []() { return CaughtSpecialFish(1000); }); // catch 1,000 special fish
	AddAchievement(147u, []() { return CaughtSpecialFish(10000); }); // catch 10,000 special fish
	AddAchievement(148u, []() { return CaughtSpecialFish(100000); }); // catch 100,000 special fish

	// premium fish
	AddAchievement(149u, []() { return SaveData::saveData.currencyList.at(50u).numOwned >= 1; }); // catch 1 premium fish
	AddAchievement(150u, []() { return SaveData::saveData.currencyList.at(50u).numOwned >= 10; }); // catch 10 premium fish
	AddAchievement(151u, []() { return SaveData::saveData.currencyList.at(50u).numOwned >= 100; }); // catch 100 premium fish
	AddAchievement(152u, []() { return SaveData::saveData.currencyList.at(50u).numOwned >= 1000; }); // catch 1,000 premium fish
	AddAchievement(153u, []() { return SaveData::saveData.currencyList.at(50u).numOwned >= 10000; }); // catch 10,000 premium fish

	// catch all fish
	AddAchievement(154u, []() { // catch all the fish in the forest
		for (auto& [id, data] : SaveData::data.fishData) {
			if (data.worldId == 53u && SaveData::saveData.fishData.at(id).calcTotalCaughtFish() == 0)
				return false;
		}
		return true;
	});
	AddAchievement(155u, []() { // catch all the special fish in the forest
		for (auto& [id, data] : SaveData::data.fishData) {
			if (data.worldId == 53u && data.isRareFish && SaveData::saveData.fishData.at(id).calcTotalCaughtFish() == 0)
				return false;
		}
		return true;
	});
	AddAchievement(156u, []() { // catch all fish qualities on all fish in the forest
		for (auto& [id, data] : SaveData::data.fishData) {
			if (data.worldId == 53u) {
				for (double caught : SaveData::saveData.fishData.at(id).totalNumOwned) {
					if (caught == 0)
						return false;
				}
			}
		}
		return true;
	});

	// premium buffs
	AddAchievement(157u, []() { return Main::premiumBuffList.size() >= 2; }); // have 2 premium buffs active at the same time
	AddAchievement(158u, []() { return Main::premiumBuffList.size() >= 3; }); // have 3 premium buffs active at the same time

	// get all quality on a single fish
	AddAchievement(159u, []() { // get all three stars on a single fish
		for (auto& [id, saveData] : SaveData::saveData.fishData) {
			bool allCaught = true;
			for (double caught : saveData.totalNumOwned) {
				if (caught == 0) {
					allCaught = false;
					break;
				}
			}
			if (allCaught)
				return true;
		}
		return false;
	});
	AddAchievement(160u, []() {  // get all three stars on a single special fish
		for (auto& [id, saveData] : SaveData::saveData.fishData) {
			if (!SaveData::data.fishData.at(id).isRareFish)
				continue;

			bool allCaught = true;
			for (double caught : saveData.totalNumOwned) {
				if (caught == 0) {
					allCaught = false;
					break;
				}
			}
			if (allCaught)
				return true;
		}
		return false;
	});

	// quality
	// bronze
	AddAchievement(161u, []() { return CheckQuality(1, 1); }); // catch a bronze quality fish
	AddAchievement(162u, []() { return CheckQuality(100, 1); }); // catch 100 bronze quality fish
	AddAchievement(163u, []() { return CheckQuality(10000, 1); }); // catch 10,000 bronze quality fish
	AddAchievement(164u, []() { return CheckQuality(50000, 1); }); // catch 50,000 bronze quality fish
	AddAchievement(165u, []() { return CheckQuality(150000, 1); }); // catch 150,000 bronze quality fish
	// silver
	AddAchievement(166u, []() { return CheckQuality(1, 2); }); // catch a silver quality fish
	AddAchievement(167u, []() { return CheckQuality(100, 2); }); // catch 100 silver quality fish
	AddAchievement(168u, []() { return CheckQuality(1000, 2); }); // catch 1,000 silver quality fish
	AddAchievement(169u, []() { return CheckQuality(10000, 2); }); // catch 10,000 silver quality fish
	AddAchievement(170u, []() { return CheckQuality(60000, 2); }); // catch 60,000 silver quality fish
	// gold
	AddAchievement(171u, []() { return CheckQuality(1, 3); }); // catch a gold quality fish
	AddAchievement(172u, []() { return CheckQuality(10, 3); }); // catch 10 gold quality fish
	AddAchievement(173u, []() { return CheckQuality(100, 3); }); // catch 100 gold quality fish
	AddAchievement(174u, []() { return CheckQuality(1000, 3); }); // catch 1,000 gold quality fish
	AddAchievement(175u, []() { return CheckQuality(10000, 3); }); // catch 10,000 gold quality fish

	// catch biggest fish
	AddAchievement(176u, []() { // catch the biggest size of a fish
		for (auto& [id, data] : SaveData::data.fishData) {
			if (SaveData::saveData.fishData.at(id).biggestSizeCaught >= SaveData::data.fishData.at(id).maxSize)
				return true;
		}
		return false;
	});
	AddAchievement(177u, []() { // catch the smallest size of a fish
		for (auto& [id, data] : SaveData::data.fishData) {
			if (SaveData::saveData.fishData.at(id).smallestSizeCaught <= SaveData::data.fishData.at(id).minSize)
				return true;
		}
		return false;
	});
	AddAchievement(178u, []() { // catch the biggest size of all fish in the forest
		for (auto& [id, data] : SaveData::data.fishData) {
			if (SaveData::saveData.fishData.at(id).biggestSizeCaught < data.maxSize)
				return false;
		}
		return true;
	});

	// max forest
	AddAchievement(179u, []() { // max out a singular fish (max size size and all qualities)
		for (auto& [id, saveData] : SaveData::saveData.fishData) {
			bool allCaught = true;
			for (double caught : saveData.totalNumOwned) {
				if (caught == 0) {
					allCaught = false;
					break;
				}
			}
			if (allCaught && saveData.biggestSizeCaught >= SaveData::data.fishData.at(id).maxSize)
				return true;
		}
		return false;
	});
	AddAchievement(180u, []() { // catch max size and max quality of all fish in the forest
		for (auto& [id, saveData] : SaveData::saveData.fishData) {
			bool allCaught = true;
			for (double caught : saveData.totalNumOwned) {
				if (caught == 0) {
					allCaught = false;
					break;
				}
			}
			if (!allCaught || saveData.biggestSizeCaught < SaveData::data.fishData.at(id).maxSize)
				return false;
		}
		return true;
	});

	// make money
	AddAchievement(181u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1000; }); // make 1,000 money
	AddAchievement(182u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 100000; }); // make 100,000 money
	AddAchievement(183u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1000000; }); // make 1m money
	AddAchievement(184u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1e8; }); // make 100m money
	AddAchievement(185u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1e9; }); // make 1b money
	AddAchievement(186u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1e11; }); // make 100b money
	AddAchievement(187u, []() { return SaveData::saveData.currencyList.at(53u).totalNumOwned >= 1e12; }); // make 1t money

	// money per second
	AddAchievement(188u, []() { return CalcMoneyPerSecond() >= 1.0; }); // make 1 money per second
	AddAchievement(189u, []() { return CalcMoneyPerSecond() >= 10.0; }); // make 10 money per second
	AddAchievement(190u, []() { return CalcMoneyPerSecond() >= 100.0; }); // make 100 money per second
	AddAchievement(191u, []() { return CalcMoneyPerSecond() >= 1000.0; }); // make 1,000 money per second
	AddAchievement(192u, []() { return CalcMoneyPerSecond() >= 10000.0; }); // make 10,000 money per second

	// purchase auto fishers
	AddAchievement(193u, []() { return world::currWorld->autoFisherList.size() >= 1; }); // purchase first auto fisher
	AddAchievement(194u, []() { return world::currWorld->autoFisherList.size() >= 5; }); // purchase 5 auto fishers
	AddAchievement(195u, []() { return world::currWorld->autoFisherList.size() >= SaveData::orderedData.autoFisherData.size(); }); // purchase all auto fishers

	// auto fisher upgrades
	AddAchievement(196u, []() { // max upgrade an auto fisher
		for (uint32_t id : SaveData::orderedData.autoFisherData) {
			ProgressionNode& data = SaveData::data.progressionData.at(id);
			SaveEntry& saveData = SaveData::saveData.progressionData.at(id);
			if (saveData.level >= data.maxLevel)
				return true;
		}
		return false;
	});
	AddAchievement(197u, []() { // max upgrade 5 auto fishers
		int maxCount = 0;
		for (uint32_t id : SaveData::orderedData.autoFisherData) {
			ProgressionNode& data = SaveData::data.progressionData.at(id);
			SaveEntry& saveData = SaveData::saveData.progressionData.at(id);
			if (saveData.level >= data.maxLevel) {
				if (++maxCount >= 5)
					return true;
			}
		}
		return false;
	});
	AddAchievement(198u, []() { // max upgrade 10 auto fishers
		for (uint32_t id : SaveData::orderedData.autoFisherData) {
			ProgressionNode& data = SaveData::data.progressionData.at(id);
			SaveEntry& saveData = SaveData::saveData.progressionData.at(id);
			if (saveData.level < data.maxLevel)
				return false;
		}
		return true;
	});

	// fish schools
	AddAchievement(199u, []() { return SaveData::saveData.fishFromSchools >= 1; }); // fish in a fish school
	AddAchievement(200u, []() { return SaveData::saveData.fishFromSchools >= 500; }); // fish in 500 fish schools
	AddAchievement(201u, []() { return SaveData::saveData.fishFromSchools >= 10000; }); // fish in 10000 fish schools

	// baits
	AddAchievement(202u, []() { // purchase your first bait
		for (uint32_t id : SaveData::orderedData.baitData)
			if (SaveData::saveData.progressionData.at(id).level != 0)
				return true;
		return false;
	}); 
	AddAchievement(203u, []() { // purchase all baits
		for (uint32_t id : SaveData::orderedData.baitData)
			if (SaveData::saveData.progressionData.at(id).level == 0)
				return false;
		return true;
	});

	// pets
	AddAchievement(204u, []() { // unlock a pet
		for (uint32_t id : SaveData::orderedData.petData)
			if (SaveData::saveData.progressionData.at(id).level != 0)
				return true;
		return false;
	});
	AddAchievement(205u, []() { // buy all pets
		for (uint32_t id : SaveData::orderedData.petData)
			if (SaveData::saveData.progressionData.at(id).level == 0)
				return false;
		return true;
	});

	AddAchievement(206u, []() { return SaveData::saveData.progressionData.at(90u).level >= 1; }); // hire fish transporter
	AddAchievement(207u, []() { return SaveData::saveData.progressionData.at(90u).level >= SaveData::data.progressionData.at(90u).maxLevel; }); // max level fish transporter
}

void Achievements::CheckGroup(AchievementTrigger trigger) {
	for (uint32_t id : achievementsPerTrigger.at(trigger)) {
		SaveEntry& saveData = SaveData::saveData.achievementList.at(id);
		if (saveData.level == 0 && achievements.at(id)()) { // if not unlocked && condition is met
			ModifierNode& modifierData = SaveData::data.modifierData.at(id);
			saveData.level = 1; // unlock the data
			Upgrades::MarkDirty(modifierData.stats); // make the stat dirty
			NotifyPlayer(id);
			std::cout << "you have unlocked achievement!\n";
		}
	}
}

void Achievements::AddAchievement(uint32_t id, std::function<bool()> condition) {
	// add condition to achievement map
	achievements.insert({ id, condition });

	// add id to achievement map sorted by triggers
	AchievementTrigger trigger = SaveData::data.achievementData.at(id).trigger;
	achievementsPerTrigger[trigger].insert(id);
}

void Achievements::NotifyPlayer(uint32_t id) {
	FachievementStruct& data = SaveData::data.achievementData.at(id);
	Main::achievementUnlocked->start(data);
	Main::achievementWidget->updateAchievementIcon(id);
}

double Achievements::CalcMoneyPerSecond() {
	double total = 0.0;
	for (auto& autoFisher : world::currWorld->autoFisherList)
		total += autoFisher->calcMPS();
	return total;
}

bool Achievements::CheckQuality(double caughtNum, int quality) {
	double total = 0.0;
	for (auto& [id, saveData] : SaveData::saveData.fishData) {
		total += saveData.totalNumOwned[quality];
		if (total > caughtNum)
			return true;
	}
	return false;
}

bool Achievements::CaughtSpecialFish(double caughtNum) {
	double total = 0.0;
	for (auto& [id, data] : SaveData::data.fishData) {
		if (data.isRareFish) {
			total += SaveData::saveData.fishData.at(id).calcTotalCaughtFish();
			if (total >= caughtNum)
				return true;
		}
	}
	return false;
}