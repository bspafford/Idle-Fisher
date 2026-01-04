#include "heldFishWidget.h"

#include <iostream>

#include "saveData.h"
#include "upgrades.h"
#include "fishNumWidget.h"
#include "Rectangle.h"
#include "verticalBox.h"

#include "debugger.h"

UheldFishWidget::UheldFishWidget(widget* parent) : widget(parent) {
	line = std::make_unique<URectangle>(this, vector{ 0.f, 0.f }, vector{ 27.f, 1.f }, false);

	vertBox = std::make_unique<verticalBox>(this);

	SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	SetPivot({ 0.f, 1.f });
}

void UheldFishWidget::updateList(std::vector<FsaveFishData> saveFishList) {
	if (saveFishList.size() == 0)
		saveFishList = SaveData::saveData.fishData;
	fishList = saveFishList;

	// setup fish
	vertBox->RemoveAllChildren();
	fishList = removeUnneededFish();
	fishNumList.clear();

	float yOffset = 6.f;
	for (int i = 0; i < fishList.size(); i++) { // loop through fish
		for (int j = 0; j < fishList[i].numOwned.size(); j++) { // loop through fish qualities
			if (fishList[i].numOwned[j] == 0) // if no fish of this quality, skip
				continue;

			// setup fishNumWidget
			std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
			widget->setup(&SaveData::data.fishData[fishList[i].id], &fishList[i], j);
			vertBox->addChild(widget.get(), widget->getSize().y + yOffset);
			fishNumList.push_back(std::move(widget));
		}
	}

	vertBox->addChild(line.get(), line->getSize().y);

	// setup currency
	getCurrency();
	currencyList.clear();
	for (int i = 0; i < currency.size(); i++) {
		std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
		FcurrencyStruct* currencyStruct = &SaveData::data.currencyData[currency[i].x];
		widget->setup(currencyStruct, currency[i].y);
		vertBox->addChild(widget.get(), widget->getSize().y + yOffset);
		currencyList.push_back(std::move(widget));
	}

	
	vector size = { fishNumList[0]->getSize().x, vertBox->getOverflowSize() };
	setSize(size);
	setLoc(loc);
	vertBox->setLocAndSize(absoluteLoc, size);
}

void UheldFishWidget::draw(Shader* shaderProgram) {
	vertBox->draw(shaderProgram);
}

std::vector<FsaveFishData> UheldFishWidget::removeUnneededFish() {
	std::vector<FsaveFishData> data;
	for (int i = 0; i < fishList.size(); i++) {
		if (fishList[i].calcCaughtFish() > 0)
			data.push_back(fishList[i]);
	}

	return data;
}

void UheldFishWidget::getCurrency() {
	// x == id, y == num
	currency.clear();
	std::vector<vector> currencyList;

	for (int i = 0; i < fishList.size(); i++) {
		FsaveFishData saveFish = fishList[i];
		FfishData fish = SaveData::data.fishData[saveFish.id];

		for (int j = 0; j < fishList[i].numOwned.size(); j++) {
			int index = currencyInList(fish.currencyId, currencyList);
			if (index != -1) { // in list
				currencyList[index].y += float(upgrades::getFishSellPrice(fish, j) * saveFish.numOwned[j]);
			} else { // need to add it to list
				currencyList.push_back({ (float)fish.currencyId, float(upgrades::getFishSellPrice(fish, j) * saveFish.numOwned[j]) });
			}
		}
	}

	currency = currencyList;
}

int UheldFishWidget::currencyInList(int id, std::vector<vector> currencyList) {
	for (int i = 0; i < currencyList.size(); i++) {
		if (currencyList[i].x == id)
			return i;
	}

	return -1;
}