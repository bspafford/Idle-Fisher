#include "heldFishWidget.h"

#include <iostream>

#include "main.h"
#include "stuff.h"
#include "saveData.h"
#include "upgrades.h"
#include "fishNumWidget.h"
#include "Rectangle.h"

#include "debugger.h"

UheldFishWidget::UheldFishWidget(widget* parent) : widget(parent) {
	line = std::make_unique<URectangle>(vector{ 0, 0 }, vector{ 20, 1 }, false);

	SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	SetPivot({ 0.f, 1.f });
}

UheldFishWidget::~UheldFishWidget() {

}

void UheldFishWidget::updateList(std::vector<FsaveFishData> saveFishList) {
	if (saveFishList.size() == 0)
		saveFishList = SaveData::saveData.fishData;
	fishList = saveFishList;

	float yOffset = 1;
	// setup fish
	fishList = removeUnneededFish();
	fishNumList.clear();

	for (int i = 0; i < fishList.size(); i++) {
		for (int j = 0; j < fishList[i].numOwned.size(); j++) {
			if (fishList[i].numOwned[j] == 0)
				continue;

			// setup fishNumWidget
			std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
			widget->SetPivot(pivot);

			widget->setLoc({ widget->getLoc().x, yOffset });
			widget->setup(&SaveData::data.fishData[fishList[i].id], &fishList[i], j);
			yOffset -= widget->getSize().y + 1;

			fishNumList.push_back(std::move(widget));
		}
	}

	// adjusted for the line
	yOffset -= 5;

	// setup currency
	getCurrency();
	currencyList.clear();
	for (int i = 0; i < currency.size(); i++) {
		std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
		widget->SetPivot(pivot);

		FcurrencyStruct* currencyStruct = &SaveData::data.currencyData[currency[i].x];

		widget->setLoc({ widget->getLoc().x, yOffset });
		widget->setup(currencyStruct, currency[i].y);
		yOffset -= widget->getSize().y + 1.f;

		currencyList.push_back(std::move(widget));
	}

	size.x = 35.f;
	size.y = yOffset;
}

void UheldFishWidget::draw(Shader* shaderProgram) {
	vector offset = vector{ 5.f, 5.f };

	float yOffset = absoluteLoc.y - 6.f;
	for (int i = 0; i < fishNumList.size(); i++) {
		fishNumList[i]->setLoc(vector{ absoluteLoc.x, yOffset } + offset);
		fishNumList[i]->draw(shaderProgram);
		yOffset -= fishNumList[i]->getSize().y + 1.f;
	}

	// draws line between fish and currency
	if (fishNumList.size() > 0) {
		line->setLoc(vector{ absoluteLoc.x, yOffset } + offset);
		line->draw(shaderProgram);

		// adjusted for the line
		yOffset -= 5.f;
	}

	for (int i = 0; i < currencyList.size(); i++) {
		currencyList[i]->setLoc(vector{ absoluteLoc.x, yOffset } + offset);
		currencyList[i]->draw(shaderProgram);
		yOffset -= currencyList[i]->getSize().y + 1.f;
	}

	size.y = yOffset;
}

std::vector<FsaveFishData> UheldFishWidget::removeUnneededFish() {
	std::vector<FsaveFishData> temp;
	for (int i = 0; i < fishList.size(); i++) {
		if (fishList[i].calcCaughtFish() > 0)
			temp.push_back(fishList[i]);
	}

	return temp;
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