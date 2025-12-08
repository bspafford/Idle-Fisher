#include "dumpster.h"

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

dumpster::dumpster(vector loc) {
	this->loc = loc;
	img = std::make_unique<Image>("./images/dumpster.png", loc, true);
	img->setUseAlpha(true);
}

void dumpster::onHover() {
	bool prevMouseOver = bMouseOver;
	bMouseOver = img->isMouseOver(true);
	if (bMouseOver)
		IHoverable::setHoveredItem(this);
	if (!prevMouseOver && bMouseOver) {
		img->setImage("./images/dumpsterHovered.png");
	} else if (prevMouseOver && !bMouseOver) {
		img->setImage("./images/dumpster.png");
	}

	if (bMouseOver && Input::getMouseButtonDown(MOUSE_BUTTON_LEFT) && !widget::getCurrWidget())
		Input::setLeftClick(this, &dumpster::sellFish);
}

void dumpster::draw(Shader* shaderProgram) {
	onHover();
	img->draw(shaderProgram);
}

void dumpster::sellFish() {
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