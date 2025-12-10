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
	closeButton->addCallback<widget>(this, &NPCwidget::removeFromViewport);
	npcImg = std::make_unique<Image>("./images/widget/npcbuttons/fisherman.png", vector{ 100, 100 }, false);

	name = std::make_unique<text>(this, "---", "biggerStraight", vector{ 0, 0 });
	buffText = std::make_unique<text>(this, "---", "straight", vector{ 0, 0 });
	buffText->setTextColor(0, 255, 0);
	debuffText = std::make_unique<text>(this, "---", "straight", vector{ 0, 0 });
	debuffText->setTextColor(255, 0, 0);
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8 * stuff::pixelSize);
	nameHolder->addChild(buffText.get(), 7 * stuff::pixelSize);
	nameHolder->addChild(debuffText.get(), 7 * stuff::pixelSize);

	upgradeBackground = std::make_unique<Image>("./images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	infoBackground = std::make_unique<Image>("./images/widget/infoBackground.png", vector{ 0, 0 }, false);
	npcBackground = std::make_unique<Image>("./images/widget/npcBackground.png", vector{ 0, 0 }, false);

	selectedIcon = std::make_unique<Image>("./images/widget/selectedFisherIcon.png", vector{ 0, 0 }, false);

	// fishing rod page
	fishingRodThumbnail = std::make_unique<Image>("./images/widget/thumbnails/fishingRodThumbnail1.png", vector{ 0, 0 }, false);
	fishingRodThumbnail->setSize(fishingRodThumbnail->getSize() * 2.f * stuff::pixelSize);
	powerText = std::make_unique<text>(this, "Power", "straight", vector{ 0, 0 });
	powerUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	powerUpgradeButton->addCallback(this, &UfishermanWidget::upgradePower);
	speedText = std::make_unique<text>(this, "Speed", "straight", vector{ 0, 0 });
	speedUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	speedUpgradeButton->addCallback(this, &UfishermanWidget::upgradeSpeed);
	catchChanceText = std::make_unique<text>(this, "Catch Chance", "straight", vector{ 0, 0 });
	catchChanceUpgradeButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{ 0, 0 }, false, false);
	catchChanceUpgradeButton->addCallback(this, &UfishermanWidget::upgradeCatchChance);

	powerButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	speedButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	catchChanceButtonPrice = std::make_unique<text>(this, "0.00k", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	powerLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 });
	speedLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 });
	catchChanceLevelText = std::make_unique<text>(this, "0", "straight", vector{ 0, 0 });

	// fishing rod stats
	powerStatsText = std::make_unique<text>(this, "Power", "straight", vector{ 0, 0 });
	powerStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speedStatsText = std::make_unique<text>(this, "Speed", "straight", vector{ 0, 0 });
	speedStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	catchChanceStatsText = std::make_unique<text>(this, "Catch Chance", "straight", vector{ 0, 0 });
	catchChanceStatsTextNum = std::make_unique<text>(this, "00", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);

	setup();
	//setupLocs();
}

UfishermanWidget::~UfishermanWidget() {

}

void UfishermanWidget::setup() {
	// setup bait
	baitHolderList = std::make_unique<UscrollBox>(this);
	baitHolderList->setVisibility(false);
	for (int i = 0; i < SaveData::data.baitData.size(); i++) {
		FbaitStruct* currData = &SaveData::data.baitData[i];
		FsaveBaitStruct* currSaveData = &SaveData::saveData.baitList[currData->id];

		std::unique_ptr<UupgradeBox> upgradeBox = std::make_unique<UupgradeBox>(baitHolderList.get(), this, currData, currSaveData);
		if (upgradeBox->buyButton)
			upgradeBox->buyButton->setParent(baitHolderList.get());
		baitHolderList->addChild(upgradeBox.get(), upgradeBox->getSize().y);
		upgradeBoxList.push_back(std::move(upgradeBox));
	}

	// set view box images
	std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "widget/thumbnails/fishingRodThumbnail1.png", 16, 16, 1, vector{ 0, 0 }, false, false);
	buttonList.push_back(std::move(button));
	std::unique_ptr<Ubutton> button2 = std::make_unique<Ubutton>(this, "widget/thumbnails/worm.png", 16, 16, 1, vector{ 0, 0 }, false, false);
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

	vector selectedPos = buttonList[selectedPageIndex]->getLoc();
	selectedIcon->setLoc({selectedPos.x - stuff::pixelSize, selectedPos.y - stuff::pixelSize});
	selectedIcon->draw(shaderProgram);

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	infoBackground->draw(shaderProgram);

	npcImg->draw(shaderProgram);

	if (selectedPageIndex == 0) {
		fishingRodThumbnail->draw(shaderProgram);
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

void UfishermanWidget::setNameDescription(std::string nameString, std::string buffString, std::string debuffString) {
	name->setText(nameString);
	buffText->setText(buffString);
	debuffText->setText(debuffString);
	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + stuff::pixelSize);
}

void UfishermanWidget::setupLocs() {
	__super::setupLocs();

	float x = npcBackground->getSize().x + 1 * stuff::pixelSize;
	float y = npcBackground->getSize().y + 1 * stuff::pixelSize;
	vector size = vector{ x, 0 } + upgradeBackground->getSize();
	vector center = { stuff::screenSize.x / 2, stuff::screenSize.y / 2 };
	vector topLeft = center - size / 2.f;
	npcBackground->setLoc(topLeft);
	infoBackground->setLoc(topLeft + vector{ 0, y });
	upgradeBackground->setLoc(topLeft + vector{ x, 0 });

	vector npcBackgroundSize = npcBackground->getSize();
	vector npcSize = npcImg->getSize();
	npcImg->setLoc(npcBackground->getLoc() + vector{npcBackgroundSize.x / 2, npcBackgroundSize.y} - vector{npcSize.x / 2, npcSize.y} - vector{0, 3 * stuff::pixelSize});

	vector baitHolderLoc = upgradeBackground->getLoc() + vector{ 4, 3 } * stuff::pixelSize;
	baitHolderList->setLocAndSize(baitHolderLoc, upgradeBackground->getSize() - vector{ 0, 6 * stuff::pixelSize });
	baitHolderList->ogLoc = baitHolderLoc;

	for (int i = 0; i < buttonList.size(); i++) {
		vector worldButtonSize = buttonList[i]->getSize();
		buttonList[i]->setLoc(upgradeBackground->getLoc() + vector{ (worldButtonSize.x + stuff::pixelSize) * i + stuff::pixelSize, -worldButtonSize.y - stuff::pixelSize });
	}

	vector closeButtonSize = closeButton->getSize();
	closeButton->setLoc({ float(upgradeBackground->getLoc().x + upgradeBackground->w * stuff::pixelSize - closeButtonSize.x / 2), float(upgradeBackground->getLoc().y - closeButtonSize.y / 2) });


	nameHolder->setLocAndSize({ float(infoBackground->getLoc().x) + 6 * stuff::pixelSize, float(infoBackground->getLoc().y) + 9 * stuff::pixelSize }, vector{ float(infoBackground->w), float(infoBackground->h) } *stuff::pixelSize);
	name->setLineLength((infoBackground->w - 10) * stuff::pixelSize);
	buffText->setLineLength((infoBackground->w - 10) * stuff::pixelSize);
	debuffText->setLineLength((infoBackground->w - 10) * stuff::pixelSize);

	fishingRodThumbnail->setLoc(stuff::screenSize / 2.f - fishingRodThumbnail->getSize() / 2.f + vector{ 0, 0 } * stuff::pixelSize);
	vector offset = { 0, 30 * stuff::pixelSize };
	vector buttonOffset = (-powerUpgradeButton->getSize() / 2.f).floor() + vector{ 90 * stuff::pixelSize, roundf(powerText->getSize().y / 2.f) };
	powerText->setLoc(stuff::screenSize / 2.f + vector{ 50, -27 } * stuff::pixelSize);
	speedText->setLoc(powerText->getLoc() + offset);
	catchChanceText->setLoc(speedText->getLoc() + offset);
	powerUpgradeButton->setLoc(powerText->getLoc() + buttonOffset);
	speedUpgradeButton->setLoc(speedText->getLoc() + buttonOffset);
	catchChanceUpgradeButton->setLoc(catchChanceText->getLoc() + buttonOffset);

	powerButtonPrice->setLoc(powerUpgradeButton->getLoc() + (powerUpgradeButton->getSize() / 2.f).floor());
	speedButtonPrice->setLoc(speedUpgradeButton->getLoc() + (speedUpgradeButton->getSize() / 2.f).floor());
	catchChanceButtonPrice->setLoc(catchChanceUpgradeButton->getLoc() + (catchChanceUpgradeButton->getSize() / 2.f).floor());

	vector levelTextOffset = { -10 * stuff::pixelSize, 0 };
	powerLevelText->setLoc(vector{ powerUpgradeButton->getLoc().x, powerText->getLoc().y } + levelTextOffset);
	speedLevelText->setLoc(vector{ speedUpgradeButton->getLoc().x, speedText->getLoc().y } + levelTextOffset);
	catchChanceLevelText->setLoc(vector{ catchChanceUpgradeButton->getLoc().x, catchChanceText->getLoc().y } + levelTextOffset);

	// fishing rod stats
	vector textOffset = { 0, 10 * stuff::pixelSize };
	vector textNumOffset = { 100 * stuff::pixelSize, 0 };
	powerStatsText->setLoc((stuff::screenSize / 2.f).floor() + vector{ -475, 30 });
	speedStatsText->setLoc(powerStatsText->getLoc() + textOffset);
	catchChanceStatsText->setLoc(speedStatsText->getLoc() + textOffset);
	powerStatsTextNum->setLoc(powerStatsText->getLoc() + textNumOffset);
	speedStatsTextNum->setLoc(speedStatsText->getLoc() + textNumOffset);
	catchChanceStatsTextNum->setLoc(catchChanceStatsText->getLoc() + textNumOffset);
}

void UfishermanWidget::upgradePower() {
	double cost = upgrades::calcFishingRodPowerPrice();
	if (SaveData::saveData.currencyList[1].numOwned >= cost) {
		SaveData::saveData.currencyList[1].numOwned -= cost;
		SaveData::saveData.fishingRod.powerLevel++;
		Main::currencyWidget->updateList();
		updateStats();
	}
}

void UfishermanWidget::upgradeSpeed() {
	double cost = upgrades::calcFishingRodSpeedPrice();
	if (SaveData::saveData.currencyList[1].numOwned >= cost) {
		SaveData::saveData.currencyList[1].numOwned -= cost;
		SaveData::saveData.fishingRod.speedLevel++;
		Main::currencyWidget->updateList();
		updateStats();
	}
}

void UfishermanWidget::upgradeCatchChance() {
	double cost = upgrades::calcFishingRodCatchChancePrice();
	if (SaveData::saveData.currencyList[1].numOwned >= cost) {
		SaveData::saveData.currencyList[1].numOwned -= cost;
		SaveData::saveData.fishingRod.catchChanceLevel++;
		Main::currencyWidget->updateList();
		updateStats();
	}
}

void UfishermanWidget::updateStats() {
	powerStatsTextNum->setText(shortNumbers::convert2Short(upgrades::calcFishingRodPower()));
	speedStatsTextNum->setText(shortNumbers::convert2Short(upgrades::calcMinFishingInterval(), true) + "s - " + shortNumbers::convert2Short(upgrades::calcMaxFishingInterval(), true) + "s");
	catchChanceStatsTextNum->setText(shortNumbers::convert2Short(upgrades::calcFishingRodCatchChance()));

	powerButtonPrice->setText(shortNumbers::convert2Short(upgrades::calcFishingRodPowerPrice()));
	speedButtonPrice->setText(shortNumbers::convert2Short(upgrades::calcFishingRodSpeedPrice()));
	catchChanceButtonPrice->setText(shortNumbers::convert2Short(upgrades::calcFishingRodCatchChancePrice()));

	powerLevelText->setText(std::to_string(SaveData::saveData.fishingRod.powerLevel));
	speedLevelText->setText(std::to_string(SaveData::saveData.fishingRod.speedLevel));
	catchChanceLevelText->setText(std::to_string(SaveData::saveData.fishingRod.catchChanceLevel));
}