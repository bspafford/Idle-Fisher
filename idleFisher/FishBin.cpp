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
	for (int i = 0; i < SaveData::saveData.fishData.size(); i++) {
		FsaveFishData* currSaveFish = &SaveData::saveData.fishData[i];
		FfishData* currFish = &SaveData::data.fishData[currSaveFish->id];

		for (int j = 0; j < SaveData::saveData.fishData[i].numOwned.size(); j++) {
			double currencyGained = currSaveFish->numOwned[j] * upgrades::getFishSellPrice(*currFish, j);
			if (currencyGained > 0)
				SaveData::saveData.currencyList[currFish->currencyId].unlocked = true;
			SaveData::saveData.currencyList[currFish->currencyId].numOwned += currencyGained;
			SaveData::saveData.currencyList[currFish->currencyId].totalNumOwned += currencyGained;
			currSaveFish->numOwned[j] = 0;
		}
	}

	Main::heldFishWidget->updateList();
	Main::currencyWidget->updateList();

	// updates UI max
	for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
		world::currWorld->autoFisherList[i]->UI->updateUI();
}