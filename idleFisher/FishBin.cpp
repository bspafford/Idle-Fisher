#include "FishBin.h"

#include "main.h"
#include "Input.h"
#include "worlds.h"
#include "saveData.h"
#include "AautoFisher.h"
#include "autoFisherUI.h"
#include "upgrades.h"
#include "achievement.h"

// widget
#include "heldFishWidget.h"
#include "currencyWidget.h"

#include "debugger.h"

FishBin::FishBin(vector loc) {
	bin = std::make_unique<Ubutton>(nullptr, "fishBin.png", 54, 66, 1, loc, true, true);
	bin->addCallback(this, &FishBin::sellFish);
	bin->SetClickAudio("temp/sellFish/fishSlap1.wav");
}

void FishBin::draw(Shader* shaderProgram) {
	bin->draw(shaderProgram);
}

void FishBin::sellFish() {
	for (auto& [fishId, currSaveFish] : SaveData::saveData.fishData) {
		FfishData* currFish = &SaveData::data.fishData.at(fishId);

		FsaveCurrencyStruct& currencyData = SaveData::saveData.currencyList.at(currFish->worldId);
		for (int j = 0; j < currSaveFish.numOwned.size(); j++) {
			double currencyGained = currSaveFish.numOwned[j] * Upgrades::Get(StatContext(Stat::FishPrice, fishId, j));
			if (currencyGained > 0)
				currencyData.unlocked = true;
			currencyData.numOwned += currencyGained;
			currencyData.totalNumOwned += currencyGained;
			currSaveFish.numOwned[j] = 0;
		}
	}

	Achievements::CheckGroup(AchievementTrigger::CurrencyEarned);


	Main::heldFishWidget->updateList(true);
	Main::currencyWidget->updateList();

	// updates UI max
	for (int i = 0; i < world::currWorld->autoFisherList.size(); i++)
		world::currWorld->autoFisherList[i]->UI->updateUI();
}

bool FishBin::calcIfPlayerInfront() {
	if (bin) {
		vector charLoc = GetCharacter()->getCharLoc();
		vector binLoc = bin->getLoc() + vector(0, 15);
		return (charLoc.y < binLoc.y);
	}
	return false;
}