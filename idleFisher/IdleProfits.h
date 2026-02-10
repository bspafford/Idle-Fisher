#pragma once

#include "worlds.h"
#include "upgrades.h"
#include "fishTransporter.h"
#include "AautoFisher.h"
#include "main.h"
#include "achievement.h"

class IdleProfits {
private:
	// sets up things like the fish transporter location
	// fills the fish transporter with currency left over from the calcIdleProfits
	// refills the auto fishers based on their fullness
	static void setup(double fishTransporterCurrency, double currencyMade) {
		for (auto& autoFisher : world::currWorld->autoFisherList)
			autoFisher->FillWithRandomFish();

		if (world::currWorld->fishTransporter) {
			world::currWorld->fishTransporter->FillWithRandomFish(fishTransporterCurrency, true);

			// calculate random fish here based on fish the autofishers can catch
			std::unordered_map<uint32_t, FsaveFishData> fishList = world::currWorld->fishTransporter->FillWithRandomFish(currencyMade, false);
			Main::idleProfitWidget->setup(fishList);
		}

		Achievements::CheckGroup(AchievementTrigger::CurrencyEarned);
	}
	
public:
	// this method doesn't calculate the path finding distance, it just uses straight lines
	// simulates the environment, walking through each step, counting up depending on how long tasks take
	static void calcIdleProfits(float timeDiff) {
		// no auto fishers, then no idle profits
		if (world::currWorld->autoFisherList.size() == 0)
			return;

		uint32_t id = 90u; // temp

		// if fish transporter isn't unlocked yet, then just fil up auto fishers
		if (SaveData::saveData.progressionData.at(id).level == 0) { // if there is no fish transporter
			for (auto& autoFisher : world::currWorld->autoFisherList) {
				uint32_t afId = autoFisher->id;
				FsaveAutoFisherStruct& saveData = SaveData::saveData.autoFisherList.at(afId);
				
				double currency = saveData.fullness + autoFisher->calcMPS(saveData.fullness) * timeDiff; // previously held + how much money per second * how many seconds since last save
				saveData.fullness = math::min(currency, Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, afId))); // clamp at capacity
			}
			setup(0, 0);
			return;
		}

		// make everything calculated per second
			// might not need to
		// keep track of when the fish transporter last at the fish transporter. so i might want to keep a map with an id as the key, then keep track of like time since start? or something?
		// so i can count up instead of down? then i just keep going until i hit how long its been
			// so like start at start, then += time to get to af1, then calculate how full af1 already is, and how much fuller it should be since it took to get there. then collect, update the time, then go to next and repeat
			// make sure to use things like fish transporter capacity so i can tell when he needs to go back to the start. then repeat until i hit the timediff

		auto& fishTransporter = world::currWorld->fishTransporter;

		double maxCapacity = Upgrades::Get(StatContext(Stat::FishTransporterMaxCapacity, id));
		double speed = Upgrades::Get(StatContext(Stat::FishTransporterSpeed, id));
		double collectSpeed = Upgrades::Get(StatContext(Stat::FishTransporterCollectSpeed, id));

		std::vector<std::unique_ptr<AautoFisher>>& autoFisherList = world::currWorld->autoFisherList;

		// keep track of what auto fisher the fish transporter is currently at
		int currAfIndex = 0; // start at 0 because thats where im going to
		// keep track of the last time i visited the autofisher (id, last time: using time since start)
		std::unordered_map<uint32_t, float> lastVisited;
		// keep track of how much currency the fish transporter is holding
		double heldCurrency = 0.0;
		// keep track of how much currency the fish transporter has sold
		double currencyMade = 0.0;
		// keep track of time
		float timeSinceStart = 0.f;
		// start at start
		vector start = fishTransporter->calcGoTo(-1);
		// see how long it takes to get to the first auto fisher
		vector destination = fishTransporter->calcGoTo(0);

		while (timeSinceStart < timeDiff) {
			// calc distance, just simple distance
			float distance = math::distance(start, destination);
			// how long does it take to walk that distance
			timeSinceStart += distance / speed;
			// now get how many fish the auto fisher had at the beginning of the save
			// and how many more fish it should now have since it took for the fish transporter to walk from the start to af1
			if (currAfIndex != -1) { // at auto fisher to collect fish
				uint32_t afId = autoFisherList[currAfIndex]->id;

				FsaveAutoFisherStruct& saveAutoFisherStruct = SaveData::saveData.autoFisherList.at(afId);
				// get the last visit time, if not in map then haven't visited yet, therefore 0
				double lastVisitTime = 0.0;
				auto it = lastVisited.find(afId);
				if (it != lastVisited.end())
					lastVisitTime = it->second;
				double autoFisherMaxCapacity = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, afId));
				// how much currency the auto fisher has made since last visit including any additional from when saved, now - last visit to get difference
				// used min to clamp at max capcity so it doesn't go over
				double mps = world::currWorld->autoFisherList[currAfIndex]->calcMPS(saveAutoFisherStruct.fullness);
				float diff = math::max(timeSinceStart - lastVisitTime, 0); // cant go below 0
				saveAutoFisherStruct.fullness = math::min(saveAutoFisherStruct.fullness + mps * diff, autoFisherMaxCapacity);
				// calculate how long it will take for the fish transporter to collect
				double collectTime = fishTransporter->calcCollectTimer(heldCurrency, saveAutoFisherStruct.fullness, true);
				// then add the additional fish the auto fisher has collected since the start of the collect time, since this is how i have it programmed now
				saveAutoFisherStruct.fullness = math::min(saveAutoFisherStruct.fullness + mps * collectTime, autoFisherMaxCapacity);
				// add collection time to time
				timeSinceStart += collectTime;

				// give the fish transporter the auto fishers fish
				// 1 of 2 things can happen: either it has enough space to collect all || there's some remainders that are left inside of the auto fisher
				if (heldCurrency + saveAutoFisherStruct.fullness > maxCapacity) {
					saveAutoFisherStruct.fullness -= maxCapacity - heldCurrency;
					heldCurrency = maxCapacity;
				} else {
					heldCurrency += saveAutoFisherStruct.fullness;
					saveAutoFisherStruct.fullness = 0;
				}

				// update the last visited time
				lastVisited[saveAutoFisherStruct.id] = timeSinceStart;

			} else { // at fish bin to sell fish
				// get how long it takes to sell the fish
				timeSinceStart += fishTransporter->calcCollectTimer(heldCurrency, 0, false);
				currencyMade += heldCurrency;
				heldCurrency = 0.0;
			}

			// now if the fish transporter is full, then need to go back to start (-1)
			// if it isn't full then can continue onto next auto fisher
				// but if next auto fisher is past size() then go to -1
			if (heldCurrency >= maxCapacity || currAfIndex + 1 >= autoFisherList.size()) { // fish transporter too full, or looping back around
				start = fishTransporter->calcGoTo(currAfIndex); // update start
				currAfIndex = -1;
				destination = fishTransporter->calcGoTo(currAfIndex); // update new destination to sell
			} else { // going to next auto fisher
				start = fishTransporter->calcGoTo(currAfIndex); // update start
				currAfIndex++; // increase af index
				destination = fishTransporter->calcGoTo(currAfIndex); // update new destination to next auto fisher
			}
		}

		// make sure to fill back up auto fishers
		for (auto& autoFisher : autoFisherList) {
			uint32_t afId = autoFisher->id;
			FsaveAutoFisherStruct& saveAutoFisherStruct = SaveData::saveData.autoFisherList.at(afId);
			double autoFisherMaxCapacity = Upgrades::Get(StatContext(Stat::AutoFisherMaxCapacity, afId));
			float diff = math::max(timeDiff - lastVisited[afId], 0); // cant go negative
			saveAutoFisherStruct.fullness = math::min(saveAutoFisherStruct.fullness + autoFisher->calcMPS(saveAutoFisherStruct.fullness) * diff, autoFisherMaxCapacity);
		}

		setup(heldCurrency, currencyMade);
	}
};