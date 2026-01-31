#include "fishermanWidget.h"

#include "main.h"
#include "npc.h"

#include "verticalBox.h"
#include "text.h"
#include "button.h"
#include "upgrades.h"
#include "shortNumbers.h"

#include "debugger.h"

UfishermanWidget::UfishermanWidget(widget* parent, npc* NPCParent) : widget(parent) {
	this->NPCParent = NPCParent;

	closeButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->SetPivot({ 0.5f, 0.5f });
	closeButton->addCallback<widget>(this, &NPCwidget::removeFromViewport);

	npcImg = std::make_unique<Image>("images/widget/npcbuttons/fisherman.png", vector{ 0, 0 }, false);
	npcImg->SetPivot({ 0.5f, 0.f });

	name = std::make_unique<text>(this, "---", "biggerStraight", vector{ 0, 0 });
	buffText = std::make_unique<text>(this, "---", "straight", vector{ 0, 0 });
	buffText->setTextColor(0, 255, 0);
	debuffText = std::make_unique<text>(this, "---", "straight", vector{ 0, 0 });
	debuffText->setTextColor(255, 0, 0);
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8);
	nameHolder->addChild(buffText.get(), 7);
	nameHolder->addChild(debuffText.get(), 7);

	npcBackground = std::make_unique<Image>("images/widget/npcBackground.png", vector{ 0, 0 }, false);
	npcBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	npcBackground->SetPivot({ 1.f, 0.f });
	infoBackground = std::make_unique<Image>("images/widget/infoBackground.png", vector{ 0, 0 }, false);
	infoBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	infoBackground->SetPivot({ 1.f, 1.f });
	upgradeBackground = std::make_unique<Image>("images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	upgradeBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	upgradeBackground->SetPivot({ 0.f, 0.5f });

	selectedIcon = std::make_unique<Image>("images/widget/selectedTabIcon.png", vector{ 0, 0 }, false);

	// fishing rod page
	fishingRodThumbnail = std::make_unique<Image>("images/widget/thumbnails/fishingRod" + std::to_string(calcFishingRodIndex() + 1) + ".png", vector{0, 0}, false);
	fishingRodThumbnail->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishingRodThumbnail->SetPivot({ 1.f, 0.5f });
	fishingLineThumbnail = std::make_unique<Image>("images/widget/thumbnails/fishingLine" + std::to_string(calcFishingLineIndex() + 1) + ".png", vector{ 0, 0 }, false);
	fishingLineThumbnail->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishingLineThumbnail->SetPivot({ 1.f, 0.5f });
	bobberThumbnail = std::make_unique<Image>("images/widget/thumbnails/bobber" + std::to_string(calcBobberIndex() + 1) + ".png", vector{ 0, 0 }, false);
	bobberThumbnail->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	bobberThumbnail->SetPivot({ 1.f, 0.5f });

	powerText = std::make_unique<text>(this, "Power", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	powerText->SetPivot({ 0.f, 0.5f });
	powerUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	powerUpgradeButton->addCallback(this, &UfishermanWidget::upgradePower);
	speedText = std::make_unique<text>(this, "Speed", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	speedText->SetPivot({ 0.f, 0.5f });
	speedUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	speedUpgradeButton->addCallback(this, &UfishermanWidget::upgradeSpeed);
	speedUpgradeButton->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	speedUpgradeButton->SetPivot({ 1.f, 0.5f });
	catchChanceText = std::make_unique<text>(this, "Catch Chance", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	catchChanceText->SetPivot({ 0.f, 0.5f });
	catchChanceUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	catchChanceUpgradeButton->addCallback(this, &UfishermanWidget::upgradeCatchChance);

	powerButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	powerButtonPrice->SetPivot({ 0.f, 0.5f });
	speedButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	speedButtonPrice->SetPivot({ 0.f, 0.5f });
	catchChanceButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	catchChanceButtonPrice->SetPivot({ 0.f, 0.5f });

	powerLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	powerLevelText->SetPivot({ 0.f, 0.5f });
	speedLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speedLevelText->SetPivot({ 0.f, 0.5f });
	catchChanceLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	catchChanceLevelText->SetPivot({ 0.f, 0.5f });

	// fishing rod stats
	powerStatsText = std::make_unique<text>(this, "Power", "straight", vector{ 0, 0 });
	powerStatsText->SetPivot({ 0.f, 1.f });
	powerStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	powerStatsTextNum->SetPivot({ 0.f, 1.f });
	speedStatsText = std::make_unique<text>(this, "Speed", "straight", vector{ 0, 0 });
	speedStatsText->SetPivot({ 0.f, 1.f });
	speedStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speedStatsTextNum->SetPivot({ 0.f, 1.f });
	catchChanceStatsText = std::make_unique<text>(this, "Catch Chance", "straight", vector{ 0, 0 });
	catchChanceStatsText->SetPivot({ 0.f, 1.f });
	catchChanceStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	catchChanceStatsTextNum->SetPivot({ 0.f, 1.f });

	setup();
}

void UfishermanWidget::setup() {
	// setup bait
	baitHolderList = std::make_unique<UscrollBox>(this);
	baitHolderList->setVisibility(false);
	for (uint32_t baitId : SaveData::orderedData.baitData) {
		ModifierNode& baitData = SaveData::data.modifierData.at(baitId);
		
		SaveEntry* baitSaveData = &SaveData::saveData.progressionData.at(baitId);

		std::unique_ptr<UupgradeBox> upgradeBox = std::make_unique<UupgradeBox>(baitHolderList.get(), this, &baitData, baitSaveData, UpgradeBoxType::Bait);
		if (upgradeBox->buyButton)
			upgradeBox->buyButton->setParent(baitHolderList.get());
		baitHolderList->addChild(upgradeBox.get(), upgradeBox->getSize().y);
		upgradeBoxList.push_back(std::move(upgradeBox));
	}

	// set view box images
	std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "widget/thumbnails/fishingRodThumbnail.png", 16, 16, 1, vector{ 0, 0 }, false, false);
	buttonList.push_back(std::move(button));
	std::unique_ptr<Ubutton> button2 = std::make_unique<Ubutton>(this, "widget/thumbnails/bait76.png", 16, 16, 1, vector{ 0, 0 }, false, false);
	buttonList.push_back(std::move(button2));

	buttonList[0]->addCallback(this, &UfishermanWidget::setPage1);
	buttonList[1]->addCallback(this, &UfishermanWidget::setPage2);

	setupLocs();
	updateStats();
}

void UfishermanWidget::showUpgradePage(int index) {
	selectedPageIndex = index;
	baitHolderList->setVisibility(index == 1);
}

void UfishermanWidget::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	if (!visible)
		return;

	CheckTextColor();

	vector selectedPos = buttonList[selectedPageIndex]->getAbsoluteLoc();
	selectedIcon->setLoc(selectedPos - vector{ 0.f, 3.f });
	selectedIcon->draw(shaderProgram);

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	infoBackground->draw(shaderProgram);

	npcImg->draw(shaderProgram);

	if (selectedPageIndex == 0) {
		fishingRodThumbnail->draw(shaderProgram);
		fishingLineThumbnail->draw(shaderProgram);
		bobberThumbnail->draw(shaderProgram);

		powerText->draw(shaderProgram);
		powerUpgradeButton->draw(shaderProgram);
		speedText->draw(shaderProgram);
		speedUpgradeButton->draw(shaderProgram);
		catchChanceText->draw(shaderProgram);
		catchChanceUpgradeButton->draw(shaderProgram);

		powerButtonPrice->draw(shaderProgram);
		speedButtonPrice->draw(shaderProgram);
		catchChanceButtonPrice->draw(shaderProgram);

		powerLevelText->draw(shaderProgram);
		speedLevelText->draw(shaderProgram);
		catchChanceLevelText->draw(shaderProgram);

		// fishing rod stats
		powerStatsText->draw(shaderProgram);
		powerStatsTextNum->draw(shaderProgram);
		speedStatsText->draw(shaderProgram);
		speedStatsTextNum->draw(shaderProgram);
		catchChanceStatsText->draw(shaderProgram);
		catchChanceStatsTextNum->draw(shaderProgram);
	} else {
		nameHolder->draw(shaderProgram);
	}

	baitHolderList->draw(shaderProgram);
	for (int i = 0; i < buttonList.size(); i++) {
		buttonList[i]->draw(shaderProgram);
	}

	closeButton->draw(shaderProgram);
}

void UfishermanWidget::setNameDescription(std::string nameString, std::string description) {
	name->setText(nameString);
	buffText->setText(description);
	//debuffText->setText(debuffString);

	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + 1.f);
	nameHolder->changeChildHeight(buffText.get(), buffText->getSize().y + 1.f);
	nameHolder->changeChildHeight(debuffText.get(), debuffText->getSize().y + 1.f);
}

void UfishermanWidget::setupLocs() {
	__super::setupLocs();

	float widgetWidth = npcBackground->getSize().x + upgradeBackground->getSize().x;
	vector center = vector{ widgetWidth / 2.f - upgradeBackground->getSize().x, 0.f };
	npcBackground->setLoc(center + vector{ -1.f, 1.f });
	infoBackground->setLoc(center + vector{ -1.f, -1.f });
	upgradeBackground->setLoc(center + vector{ 1.f, 1.f });

	if (npcImg)
		npcImg->setLoc(npcBackground->getAbsoluteLoc() + vector{ npcBackground->getSize().x / 2.f, 3.f });

	vector nameHolderSize = infoBackground->getSize() - 10.f;
	name->setLineLength(nameHolderSize.x);
	buffText->setLineLength(nameHolderSize.x);
	debuffText->setLineLength(nameHolderSize.x);
	nameHolder->setLocAndSize(infoBackground->getAbsoluteLoc() + 5.f, nameHolderSize);

	vector baitHolderPos = (upgradeBackground->getAbsoluteLoc() + vector{ 4, 3 }).floor();
	baitHolderList->setLocAndSize(baitHolderPos, upgradeBackground->getSize() - vector{ 8.f, 6.f });

	for (int i = 0; i < buttonList.size(); i++) {
		vector worldButtonSize = buttonList[i]->getSize();
		buttonList[i]->setLoc(upgradeBackground->getAbsoluteLoc() + vector{ (worldButtonSize.x + 1.f) * i + 1.f, upgradeBackground->getSize().y + 1.f});
	}

	closeButton->setLoc(upgradeBackground->getAbsoluteLoc() + upgradeBackground->getSize());

	fishingRodThumbnail->setLoc(vector{ 38.f, 0 });
	fishingLineThumbnail->setLoc(vector{ 38.f, 0 });
	bobberThumbnail->setLoc(vector{ 38.f, 0 });

	// price buttons, name, and levels
	vector offset = { -45.f, 30.f };

	speedUpgradeButton->setLoc({ widgetWidth / 2.f - 5.f, 0 });
	powerUpgradeButton->setLoc(speedUpgradeButton->getAbsoluteLoc() + vector{ 0.f, offset.y });
	catchChanceUpgradeButton->setLoc(speedUpgradeButton->getAbsoluteLoc() - vector{ 0.f, offset.y });

	powerText->setLoc(powerUpgradeButton->getAbsoluteLoc() + vector{ offset.x, powerUpgradeButton->getSize().y / 2.f });
	speedText->setLoc(speedUpgradeButton->getAbsoluteLoc() + vector{ offset.x, speedUpgradeButton->getSize().y / 2.f });

	catchChanceText->setLoc(catchChanceUpgradeButton->getAbsoluteLoc() + vector{ offset.x, catchChanceUpgradeButton->getSize().y / 2.f });

	powerButtonPrice->setLoc(powerUpgradeButton->getAbsoluteLoc() + (powerUpgradeButton->getSize() / 2.f));
	speedButtonPrice->setLoc(speedUpgradeButton->getAbsoluteLoc() + (speedUpgradeButton->getSize() / 2.f));
	catchChanceButtonPrice->setLoc(catchChanceUpgradeButton->getAbsoluteLoc() + (catchChanceUpgradeButton->getSize() / 2.f));

	float levelTextOffset = -5.f;
	powerLevelText->setLoc(powerUpgradeButton->getAbsoluteLoc() + vector{ levelTextOffset, powerUpgradeButton->getSize().y / 2.f });
	speedLevelText->setLoc(speedUpgradeButton->getAbsoluteLoc() + vector{ levelTextOffset, speedUpgradeButton->getSize().y / 2.f });
	catchChanceLevelText->setLoc(catchChanceUpgradeButton->getAbsoluteLoc() + vector{ levelTextOffset, catchChanceUpgradeButton->getSize().y / 2.f });

	// fishing rod stats
	vector textOffset = { 0.f, -10.f };
	vector textNumOffset = { 100.f, 0.f };
	powerStatsText->setLoc(infoBackground->getAbsoluteLoc() + vector{ 5.f, infoBackground->getSize().y - 5.f });
	speedStatsText->setLoc(powerStatsText->getAbsoluteLoc() + textOffset);
	catchChanceStatsText->setLoc(speedStatsText->getAbsoluteLoc() + textOffset);
	powerStatsTextNum->setLoc(powerStatsText->getAbsoluteLoc() + textNumOffset);
	speedStatsTextNum->setLoc(speedStatsText->getAbsoluteLoc() + textNumOffset);
	catchChanceStatsTextNum->setLoc(catchChanceStatsText->getAbsoluteLoc() + textNumOffset);
}

void UfishermanWidget::upgradePower() {
	if (Upgrades::LevelUp(114u)) {
		updateStats();
		fishingRodThumbnail->setImage("images/widget/thumbnails/fishingRod" + std::to_string(calcFishingRodIndex() + 1) + ".png");
	}
}

void UfishermanWidget::upgradeSpeed() {
	if (Upgrades::LevelUp(115u)) {
		updateStats();
		fishingLineThumbnail->setImage("images/widget/thumbnails/fishingLine" + std::to_string(calcFishingLineIndex() + 1) + ".png");
	}
}

void UfishermanWidget::upgradeCatchChance() {
	if (Upgrades::LevelUp(116u)) {
		updateStats();
		bobberThumbnail->setImage("images/widget/thumbnails/bobber" + std::to_string(calcBobberIndex() + 1) + ".png");
	}
}

void UfishermanWidget::updateStats() {
	powerStatsTextNum->setText(shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::Power)));
	speedStatsTextNum->setText(shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::FishComboSpeed)));
	catchChanceStatsTextNum->setText(shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::CatchNum)));

	
	powerCost = Upgrades::GetPrice(114u);
	speedCost = Upgrades::GetPrice(115u);
	chanceCost = Upgrades::GetPrice(116u);
	powerButtonPrice->setText(shortNumbers::convert2Short(powerCost));
	speedButtonPrice->setText(shortNumbers::convert2Short(speedCost));
	catchChanceButtonPrice->setText(shortNumbers::convert2Short(chanceCost));

	powerLevelText->setText(std::to_string(SaveData::saveData.progressionData.at(114u).level)); // power
	speedLevelText->setText(std::to_string(SaveData::saveData.progressionData.at(115u).level)); // speed
	catchChanceLevelText->setText(std::to_string(SaveData::saveData.progressionData.at(116u).level)); // catch chance

}

void UfishermanWidget::CheckTextColor() {
	FsaveCurrencyStruct& currencyData = SaveData::saveData.currencyList.at(53u);

	// power
	if (currencyData.numOwned >= powerCost)
		powerButtonPrice->setTextColor(255, 255, 255);
	else
		powerButtonPrice->setTextColor(255, 0, 0);

	// speed
	if (currencyData.numOwned >= speedCost)
		speedButtonPrice->setTextColor(255, 255, 255);
	else
		speedButtonPrice->setTextColor(255, 0, 0);

	// catch chance
	if (currencyData.numOwned >= chanceCost)
		catchChanceButtonPrice->setTextColor(255, 255, 255);
	else
		catchChanceButtonPrice->setTextColor(255, 0, 0);
}

int UfishermanWidget::calcFishingRodIndex() {
	return SaveData::saveData.progressionData.at(114u).level / 10;
}

int UfishermanWidget::calcFishingLineIndex() {
	return SaveData::saveData.progressionData.at(115u).level / 10;
}

int UfishermanWidget::calcBobberIndex() {
	return SaveData::saveData.progressionData.at(116u).level / 10;
}