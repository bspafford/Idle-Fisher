#include "idleProfitWidget.h"
#include "main.h"
#include "button.h"
#include "text.h"
#include "heldFishWidget.h"
#include "Rectangle.h"
#include "wrapBox.h"
#include "Upgrades.h"

#include "debugger.h"

UidleProfitWidget::UidleProfitWidget(widget* parent) : widget(parent) {
	background = std::make_unique<Image>("images/widget/idleProfitBackground.png", vector{ 0.f, 0.f }, false);
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	title = std::make_unique<text>(this, "Idle Profit", "biggerStraight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	title->SetPivot({ 0.f, 1.f });

	collectButton = std::make_unique<Ubutton>(this, "widget/biggerButton.png", 31, 15, 1, vector{ 0, 0 }, false, false);
	collectButton->SetPivot({ 0.5f, 0.f });
	collectButton->addCallback<widget>(this, &widget::removeFromViewport);

	collectText = std::make_unique<text>(this, "Collect", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	collectText->SetPivot({ 0.f, 0.5f });

	fishWrapBox = std::make_unique<UwrapBox>(this, background->getAbsoluteLoc(), vector(160.f, background->getSize().y - 60));
	line = std::make_unique<URectangle>(this, vector(0, 0), vector(100.f, 2.f), false);
	line->SetAnchor(ANCHOR_CENTER, ANCHOR_BOTTOM);
	line->SetPivot(vector(0.5f, 0.5));

	currencyWrapBox = std::make_unique<UwrapBox>(this, vector(0, 0), vector(0, 0));

	setupLocs();
}

UidleProfitWidget::~UidleProfitWidget() {

}

void UidleProfitWidget::draw(Shader* shaderProgram) {
	if (background)
		background->draw(shaderProgram);

	title->draw(shaderProgram);
	collectButton->draw(shaderProgram);
	collectText->draw(shaderProgram);

	fishWrapBox->draw(shaderProgram);
	line->draw(shaderProgram);
	currencyWrapBox->draw(shaderProgram);
}

void UidleProfitWidget::setup(std::unordered_map<uint32_t, FsaveFishData> fishList) {
	std::unordered_map<uint32_t, double> currencyList;

	for (uint32_t id : SaveData::orderedData.fishData) {
		auto it = fishList.find(id);
		if (it == fishList.end())
			continue; // fish not in list

		FsaveFishData& saveFishData = it->second;
		FfishData& fishData = SaveData::data.fishData.at(id);
		std::unique_ptr<UfishNumWidget> fishBox = std::make_unique<UfishNumWidget>(this);
		fishBox->setup(&fishData, &saveFishData, 0);
		fishWrapBox->addChild(fishBox.get());
		fishNumList.push_back(std::move(fishBox));

		double currency = Upgrades::Get(StatContext(Stat::FishPrice, id, 0));
		currencyList[fishData.worldId] += currency;
	}

	for (uint32_t id : SaveData::orderedData.currencyData) {
		auto it = currencyList.find(id);
		if (it == currencyList.end())
			continue; // currency not in list
		
		FcurrencyStruct& currencyData = SaveData::data.currencyData.at(id);
		std::unique_ptr<UfishNumWidget> currencyBox = std::make_unique<UfishNumWidget>(this);
		currencyBox->setup(&currencyData, it->second);
		currencyWrapBox->addChild(currencyBox.get());
		currencyNumList.push_back(std::move(currencyBox));
	}

	line->setLoc(vector(0.f, fishWrapBox->getAbsoluteLoc().y + fishWrapBox->getOverflowSize()));
	currencyWrapBox->setLocAndSize(fishWrapBox->getAbsoluteLoc() + vector(0, fishWrapBox->getOverflowSize() + 3), fishWrapBox->getSize());

	fishWrapBox->SetPadding(vector(10.f, 10.f));
	fishWrapBox->UpdateChildren();
}

void UidleProfitWidget::setupLocs() {
	title->setLoc(background->getAbsoluteLoc() + background->getSize() / vector{ 2.f, 1.f } - vector{ 0.f, 8.f });

	collectButton->setLoc(background->getAbsoluteLoc() + vector{ background->getSize().x / 2.f, 8.f });
	collectText->setLoc(collectButton->getAbsoluteLoc() + collectButton->getSize() / 2.f);
}
