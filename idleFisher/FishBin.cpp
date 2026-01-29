#include "FishBin.h"

#include "main.h"
#include "Input.h"
#include "worlds.h"
#include "saveData.h"
#include "AautoFisher.h"
#include "autoFisherUI.h"
#include "upgrades.h"

// widget
#include "heldFishWidget.h"
#include "currencyWidget.h"

#include "debugger.h"

FishBin::FishBin(vector loc) {
	bin = std::make_unique<Ubutton>(nullptr, "fishBin.png", 54, 66, 1, loc, true, true);
	bin->addCallback(this, &FishBin::sellFish);
}

void FishBin::draw(Shader* shaderProgram) {
	bin->draw(shaderProgram);
}

void FishBin::sellFish() {
	for (auto& [fishId, currSaveFish] : SaveData::saveData.fishData) {
		FfishData* currFish = &SaveData::data.fishData.at(fishId);

		for (int j = 0; j < currSaveFish.numOwned.size(); j++) {
			double currencyGained = currSaveFish.numOwned[j] * Upgrades::Get(StatContext(Stat::FishPrice, fishId, StatContextType::Fish, j));
			if (currencyGained > 0)
				SaveData::saveData.currencyList[currFish->worldId].unlocked = true;
			SaveData::saveData.currencyList[currFish->worldId].numOwned += currencyGained;
			SaveData::saveData.currencyList[currFish->worldId].totalNumOwned += currencyGained;
			currSaveFish.numOwned[j] = 0;
		}
	}

	Main::heldFishWidget->updateList();
	Main::currencyWidget->updateList();

	// updates UI max
	for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
		world::currWorld->autoFisherList[i]->UI->updateUI();
}