#include "merchantWidget.h"

#include "main.h"
#include "npc.h"

#include "verticalBox.h"
#include "text.h"
#include "button.h"

#include "debugger.h"

UmerchantWidget::UmerchantWidget(widget* parent, npc* NPCParent) : widget(parent) {
	this->NPCParent = NPCParent;

	closeButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->SetPivot({ 0.5f, 0.5f });
	closeButton->addCallback<widget>(this, &NPCwidget::removeFromViewport);
	
	npcImg = std::make_unique<Image>("images/widget/npcButtons/merchant.png", vector{ 100, 100 }, false);
	npcImg->SetPivot({ 0.5f, 0.f });

	name = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straight", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	if (nameHolder) {
		nameHolder->addChild(name.get(), 8 * stuff::pixelSize);
		nameHolder->addChild(description.get(), 4 * stuff::pixelSize);
	}

	upgradeBackground = std::make_unique<Image>("images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	upgradeBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	upgradeBackground->SetPivot({ 0.f, 0.5f });
	infoBackground = std::make_unique<Image>("images/widget/infoBackground.png", vector{ 0, 0 }, false);
	infoBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	infoBackground->SetPivot({ 1.f, 1.f });
	npcBackground = std::make_unique<Image>("images/widget/npcBackground.png", vector{ 0, 0 }, false);
	npcBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	npcBackground->SetPivot({ 1.f, 0.f });

	selectedWorldIcon = std::make_unique<Image>("images/widget/selectedTabIcon.png", vector{ 0, 0 }, false);

	setup();
	//setupLocs();
}

UmerchantWidget::~UmerchantWidget() {
	upgradeHolderList.clear();
}

void UmerchantWidget::setup() {
	// see if world name is different from last
		// if it is then add a scroll box to list
	// else
		// add the upgrade box to the last index of the list

	// for every world add a button to the list
	for (auto& worldId : Scene::GetWorldsList()) {
		std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "currency/coin" + std::to_string(worldId) + ".png", 16, 16, 1, vector{ 0, 0 }, false, false);
		worldButtonList.push_back(std::move(button));

		std::unique_ptr<UscrollBox> scrollBox = std::make_unique<UscrollBox>(this);
		scrollBox->setVisibility(false);
		upgradeHolderList.push_back(std::move(scrollBox));
	}

	for (const uint32_t& upgradeId : SaveData::orderedData.upgradeData) {
		SaveEntry& currSaveData = SaveData::saveData.progressionData.at(upgradeId);
		ModifierNode& currData = SaveData::data.modifierData.at(upgradeId);

		std::unique_ptr<UupgradeBox> upgradeBox = std::make_unique<UupgradeBox>(upgradeHolderList[upgradeHolderList.size() - 1].get(), this, &currData, &currSaveData, UpgradeBoxType::Upgrade);
		upgradeHolderList[Scene::GetWorldIndex(upgradeId)]->addChild(upgradeBox.get(), upgradeBox->getSize().y);
		upgradeBoxList.push_back(std::move(upgradeBox));
	}
	upgradeHolderList[0]->setVisibility(true);

	if (worldButtonList.size() >= 10) {
		worldButtonList[0]->addCallback(this, &UmerchantWidget::setPage1);
		worldButtonList[1]->addCallback(this, &UmerchantWidget::setPage2);
		worldButtonList[2]->addCallback(this, &UmerchantWidget::setPage3);
		worldButtonList[3]->addCallback(this, &UmerchantWidget::setPage4);
		worldButtonList[4]->addCallback(this, &UmerchantWidget::setPage5);
		worldButtonList[5]->addCallback(this, &UmerchantWidget::setPage6);
		worldButtonList[6]->addCallback(this, &UmerchantWidget::setPage7);
		worldButtonList[7]->addCallback(this, &UmerchantWidget::setPage8);
		worldButtonList[8]->addCallback(this, &UmerchantWidget::setPage9);
		worldButtonList[9]->addCallback(this, &UmerchantWidget::setPage10);
	}

	setupLocs();
}

void UmerchantWidget::showUpgradePage(int index) {
	selectedPageIndex = index;
	for (int i = 0; i < upgradeHolderList.size(); i++)
		upgradeHolderList[i]->setVisibility(i == index);
}

void UmerchantWidget::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	if (!visible)
		return;

	vector selectedPos = worldButtonList[selectedPageIndex]->getAbsoluteLoc();
	selectedWorldIcon->setLoc(selectedPos - vector{ 1.f, 4.f });
	selectedWorldIcon->draw(shaderProgram);

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	infoBackground->draw(shaderProgram);

	npcImg->draw(shaderProgram);

	nameHolder->draw(shaderProgram);

	for (uint32_t worldId : SaveData::orderedData.worldData) {
		SaveEntry& worldSaveData = SaveData::saveData.progressionData.at(worldId);

		if (!worldSaveData.level) // if not unlocked
			continue;

		int worldIdx = Scene::GetWorldIndex(worldId);
		upgradeHolderList[worldIdx]->draw(shaderProgram);
		worldButtonList[worldIdx]->draw(shaderProgram);
	}

	closeButton->draw(shaderProgram);
}

void UmerchantWidget::setNameDescription(std::string nameString, std::string descriptionString) {
	name->setText(nameString);
	description->setText(descriptionString);

	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + 1.f);
	nameHolder->changeChildHeight(description.get(), description->getSize().y + 1.f);
}

void UmerchantWidget::setupLocs() {
	__super::setupLocs();

	float widgetWidth = npcBackground->getSize().x + upgradeBackground->getSize().x;
	vector center = vector{ widgetWidth / 2.f - upgradeBackground->getSize().x, 0.f };
	npcBackground->setLoc(center + vector{ -1.f, 1.f });
	infoBackground->setLoc(center + vector{ -1.f, -1.f });
	upgradeBackground->setLoc(center + vector{ 1.f, 1.f });

	if (npcImg)
		npcImg->setLoc(npcBackground->getAbsoluteLoc() + vector{ npcBackground->getSize().x / 2.f, 3.f });

	nameHolder->setLocAndSize({ float(infoBackground->getLoc().x) + 6 * stuff::pixelSize, float(infoBackground->getLoc().y) + 9 * stuff::pixelSize }, vector{ float(infoBackground->getSize().x), float(infoBackground->getSize().y) } * stuff::pixelSize);
	name->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);
	description->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);

	vector nameHolderSize = infoBackground->getSize() - 10.f;
	name->setLineLength(nameHolderSize.x);
	description->setLineLength(nameHolderSize.x);
	nameHolder->setLocAndSize(infoBackground->getAbsoluteLoc() + 5.f, nameHolderSize);


	vector upgradeHolderLoc = (upgradeBackground->getAbsoluteLoc() + vector{ 4, 3 }).floor();
	for (int i = 0; i < upgradeHolderList.size(); i++) {
		upgradeHolderList[i]->setLocAndSize(upgradeHolderLoc, upgradeBackground->getSize() - vector{ 8.f, 6.f });

		if (worldButtonList.size() > i) {
			vector worldButtonSize = worldButtonList[i]->getSize();
			worldButtonList[i]->setLoc(upgradeBackground->getAbsoluteLoc() + vector{ (worldButtonSize.x + 1.f) * i + 1.f, upgradeBackground->getSize().y + 1.f });
		}
	}

	if (closeButton)
		closeButton->setLoc(upgradeBackground->getAbsoluteLoc() + upgradeBackground->getSize());
}