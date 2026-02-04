#include "heldFishWidget.h"

#include <iostream>

#include "saveData.h"
#include "upgrades.h"
#include "fishNumWidget.h"
#include "Rectangle.h"
#include "verticalBox.h"
#include "scrollBox.h"

#include "debugger.h"

UheldFishWidget::UheldFishWidget(widget* parent) : widget(parent) {
	line = std::make_unique<URectangle>(this, vector{ 0.f, 0.f }, vector{ 27.f, 1.f }, false);

	vertBox = std::make_unique<verticalBox>(this);
	fishScrollBox = std::make_unique<UscrollBox>(this);
	currencyScrollBox = std::make_unique<UscrollBox>(this);
	
	vertBox->addChild(fishScrollBox.get(), 0.f);
	vertBox->addChild(line.get(), line->getSize().y);
	vertBox->addChild(currencyScrollBox.get(), 0.f);

	SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	SetPivot({ 0.f, 1.f });
}

void UheldFishWidget::updateList(std::unordered_map<uint32_t, FsaveFishData> saveFishList) {
	if (saveFishList.size() == 0)
		saveFishList = SaveData::saveData.fishData;
	fishList = saveFishList;

	// setup fish
	fishScrollBox->RemoveAllChildren();
	currencyScrollBox->RemoveAllChildren();
	fishList = removeUnneededFish();
	fishNumList.clear();

	float biggestSizeX = 0.f;
	float yOffset = 1.f;
	for (auto& [fishId, fishData] : fishList) { // loop through fish
		for (int j = 0; j < fishData.numOwned.size(); j++) { // loop through fish qualities
			if (fishData.numOwned[j] == 0) // if no fish of this quality, skip
				continue;

			// setup fishNumWidget
			std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
			widget->setup(&SaveData::data.fishData.at(fishId), &fishData, j);
			biggestSizeX = math::max(biggestSizeX, widget->getSize().x);
			fishScrollBox->addChild(widget.get(), widget->getSize().y + yOffset);
			fishNumList.push_back(std::move(widget));
		}
	}

	// setup currency
	UpdateCurrencyMap();
	currencyList.clear();
	for (auto& [currencyId, currencyNum] : currency) {
		std::unique_ptr<UfishNumWidget> widget = std::make_unique<UfishNumWidget>(this);
		widget->setup(&SaveData::data.currencyData.at(currencyId), currencyNum);
		biggestSizeX = math::max(biggestSizeX, widget->getSize().x);
		currencyScrollBox->addChild(widget.get(), widget->getSize().y + yOffset);
		currencyList.push_back(std::move(widget));
	}
	
	setSize({ biggestSizeX, vertBox->getOverflowSize() });
	setupLocs();
}

void UheldFishWidget::draw(Shader* shaderProgram) {
	vertBox->draw(shaderProgram);
}

void UheldFishWidget::setupLocs() {
	float fishBoxY = math::min(fishScrollBox->getOverflowSize(), stuff::screenSize.y / (stuff::pixelSize * 2.f)); // either the actual size, or max out at half the screen
	float currencyBoxY = math::min(currencyScrollBox->getOverflowSize(), stuff::screenSize.y / (stuff::pixelSize * 2.f)); // either the actual size, or max out at half the screen

	fishScrollBox->setSize({ size.x, fishBoxY });
	vertBox->changeChildHeight(fishScrollBox.get(), fishScrollBox->getSize().y);
	currencyScrollBox->setSize({ size.x, currencyBoxY });
	vertBox->changeChildHeight(currencyScrollBox.get(), currencyScrollBox->getSize().y);
	setLoc(loc);
	vertBox->setLocAndSize(absoluteLoc - size / 2.f, size);
	setSize({ size.x, vertBox->getOverflowSize() });
}

std::unordered_map<uint32_t, FsaveFishData> UheldFishWidget::removeUnneededFish() {
	std::unordered_map<uint32_t, FsaveFishData> data;
	for (auto& [fishId, fishData] : fishList) {
		if (fishData.calcCaughtFish() > 0)
			data.insert({ fishId, fishData });
	}

	return data;
}

void UheldFishWidget::UpdateCurrencyMap() {
	currency.clear();

	for (auto& [fishId, saveFish] : fishList) {
		FfishData& fish = SaveData::data.fishData.at(saveFish.id);

		for (int j = 0; j < saveFish.numOwned.size(); j++)
			currency[fish.worldId] += Upgrades::Get(StatContext(Stat::FishPrice, fishId, j)) * saveFish.numOwned[j];
	}
}

int UheldFishWidget::currencyInList(uint32_t id, const std::vector<std::pair<uint32_t, double>>& currencyList) {
	for (int i = 0; i < currencyList.size(); i++) {
		if (currencyList[i].first == id)
			return i;
	}
	return -1;
}