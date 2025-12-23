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
	if (closeButton)
		closeButton->addCallback<widget>(this, &NPCwidget::removeFromViewport);
	npcImg = std::make_unique<Image>("./images/widget/npcButtons/merchant.png", vector{ 100, 100 }, false);

	name = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straight", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	if (nameHolder) {
		nameHolder->addChild(name.get(), 8 * stuff::pixelSize);
		nameHolder->addChild(description.get(), 4 * stuff::pixelSize);
	}

	upgradeBackground = std::make_unique<Image>("./images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	infoBackground = std::make_unique<Image>("./images/widget/infoBackground.png", vector{ 0, 0 }, false);
	npcBackground = std::make_unique<Image>("./images/widget/npcBackground.png", vector{ 0, 0 }, false);

	selectedWorldIcon = std::make_unique<Image>("./images/widget/selectedWorldIcon.png", vector{ 0, 0 }, false);

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
	std::string lastWorldName = "";
	int worldIndex = 1;
	for (int i = 0; i < SaveData::data.upgradeData.size(); i++) {
		FupgradeStruct* currData = &SaveData::data.upgradeData[i];
		FsaveUpgradeStruct* currSaveData = &SaveData::saveData.upgradeList[currData->id];

		if (currData->levelName != lastWorldName) {
			// for every world add a button to the list
			
			std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "currency/coin" + std::to_string(worldIndex) + ".png", 12, 12, 1, vector{ 0, 0 }, false, false);
			worldButtonList.push_back(std::move(button));

			std::unique_ptr<UscrollBox> scrollBox = std::make_unique<UscrollBox>(this);
			scrollBox->setVisibility(false);
			upgradeHolderList.push_back(std::move(scrollBox));
			lastWorldName = currData->levelName;
			
			worldIndex++;
		}

		std::unique_ptr<UupgradeBox> upgradeBox = std::make_unique<UupgradeBox>(upgradeHolderList[upgradeHolderList.size() - 1].get(), this, currData, currSaveData);
		if (upgradeBox->buyButton)
			upgradeBox->buyButton->setParent(upgradeHolderList[upgradeHolderList.size() - 1].get());
		upgradeHolderList[upgradeHolderList.size() - 1]->addChild(upgradeBox.get(), upgradeBox->getSize().y);
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

	vector selectedPos = worldButtonList[selectedPageIndex]->getLoc();
	selectedWorldIcon->setLoc({ selectedPos.x - stuff::pixelSize, selectedPos.y - stuff::pixelSize });
	selectedWorldIcon->draw(shaderProgram);

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	infoBackground->draw(shaderProgram);

	npcImg->draw(shaderProgram);

	nameHolder->draw(shaderProgram);

	for (int i = 0; i < upgradeHolderList.size(); i++) {
		if (!SaveData::saveData.worldList[i].unlocked)
			break;
		upgradeHolderList[i]->draw(shaderProgram);
		worldButtonList[i]->draw(shaderProgram);
	}

	closeButton->draw(shaderProgram);
}

void UmerchantWidget::setNameDescription(std::string nameString, std::string descriptionString) {
	name->setText(nameString);
	description->setText(descriptionString);

	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + stuff::pixelSize);
}

void UmerchantWidget::setupLocs() {
	__super::setupLocs();

	float x = (npcBackground->getSize().x + 1) * stuff::pixelSize;
	float y = (npcBackground->getSize().y + 1) * stuff::pixelSize;
	vector size = vector{ x, 0 } + upgradeBackground->getSize();
	vector center = { stuff::screenSize.x / 2, stuff::screenSize.y / 2 };
	vector topLeft = center - size / 2;
	npcBackground->setLoc(topLeft);
	infoBackground->setLoc(topLeft + vector{ 0, y });
	upgradeBackground->setLoc(topLeft + vector{ x, 0 });

	vector npcBackgroundSize = npcBackground->getSize();
	vector npcSize = npcImg->getSize();
	npcImg->setLoc(npcBackground->getLoc() + vector{npcBackgroundSize.x / 2, npcBackgroundSize.y} - vector{npcSize.x / 2, npcSize.y} - vector{0, 3 * stuff::pixelSize});

	vector upgradeHolderLoc = upgradeBackground->getLoc() + vector{ 4, 3 } * stuff::pixelSize;
	for (int i = 0; i < upgradeHolderList.size(); i++) {
		upgradeHolderList[i]->setLocAndSize(upgradeHolderLoc, upgradeBackground->getSize() - vector{0, 6} * stuff::pixelSize);
		upgradeHolderList[i]->setOgLoc(upgradeHolderLoc);

		if (worldButtonList.size() > i) {
			vector worldButtonSize = worldButtonList[i]->getSize();
			worldButtonList[i]->setLoc(upgradeBackground->getLoc() + vector{ (worldButtonSize.x + stuff::pixelSize) * i + stuff::pixelSize, -worldButtonSize.y - stuff::pixelSize });
		}
	}

	if (closeButton) {
		vector closeButtonSize = closeButton->getSize();
		closeButton->setLoc({ float(upgradeBackground->getLoc().x + upgradeBackground->getSize().x * stuff::pixelSize - closeButtonSize.x / 2), float(upgradeBackground->getLoc().y - closeButtonSize.y / 2) });
	}

	nameHolder->setLocAndSize({ float(infoBackground->getLoc().x) + 6 * stuff::pixelSize, float(infoBackground->getLoc().y) + 9 * stuff::pixelSize }, vector{ float(infoBackground->getSize().x), float(infoBackground->getSize().y) } *stuff::pixelSize);
	name->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);
	description->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);
}