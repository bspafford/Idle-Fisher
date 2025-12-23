#include "mechanicWidget.h"

#include "main.h"
#include "npc.h"
#include "currencyWidget.h"
#include "shortNumbers.h"

#include "verticalBox.h"
#include "text.h"
#include "button.h"
#include "progressBar.h"

#include "worlds.h"
#include "fishTransporter.h"

#include "debugger.h"

UmechanicWidget::UmechanicWidget(widget* parent, npc* NPCParent) : widget(parent) {
	this->NPCParent = NPCParent;

	int id = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	saveMechanicStruct = &SaveData::saveData.mechanicStruct[id];
	mechanicStruct = &SaveData::data.mechanicStruct[id];

	closeButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	closeButton->addCallback<widget>(this, &UmechanicWidget::removeFromViewport);
	npcImg = std::make_unique<Image>("./images/widget/npcButtons/mechanic.png", vector{ 0, 0 }, false);

	name = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straight", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8 * stuff::pixelSize);
	nameHolder->addChild(description.get(), 4 * stuff::pixelSize);

	npcBackground = std::make_unique<Image>("./images/widget/npcBackground.png", vector{ 0, 0 }, false);
	infoBackground = std::make_unique<Image>("./images/widget/infoBackground.png", vector{ 0, 0 }, false);
	upgradeBackground = std::make_unique<Image>("./images/widget/upgradeBackground.png", vector{ 0, 0 }, false);

	// fish transporter
	buyFishTransporterButton = std::make_unique<Ubutton>(this, "widget/button.png", 27, 13, 2, vector{ 0, 0 }, false, false);
	buyFishTransporterButton->addCallback(this, &UmechanicWidget::buyFishTransporter);
	buyFishTransporterText = std::make_unique<text>(this, "Buy Fish Transporter", "biggerStraight", vector{ 0, 0 });
	buyFishTransporterPriceText = std::make_unique<text>(this, shortNumbers::convert2Short(mechanicStruct->currencyNum), "straight", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);

	// bought screen
	fishTransporterName = std::make_unique<text>(this, "Fish Transporter", "biggerStraight", vector{ 0, 0 });
	fishTransporterImg = std::make_unique<Image>("./images/npc/fishTransporter/idleSE.png", vector{ 0, 0 }, false);
	
	if (saveMechanicStruct->unlocked)
		fishTransporterImg->setColorMod(glm::vec4(1));
	else
		fishTransporterImg->setColorMod(glm::vec4(glm::vec3(0), 1.f));

	fishTransporterImg->setSize(fishTransporterImg->getSize() * 2.5f);
	level = std::make_unique<text>(this, "0/100", "biggerStraight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	levelProgress = std::make_unique<UprogressBar>(this, vector{ 125.f, 7.f }, false);
	maxHoldText = std::make_unique<text>(this, "Max Hold:", "straight", vector{ 0, 0 });
	maxHoldValue = std::make_unique<text>(this, shortNumbers::convert2Short(0), "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	speedText = std::make_unique<text>(this, "Walk Speed:", "straight", vector{ 0, 0 });
	speedValue = std::make_unique<text>(this, shortNumbers::convert2Short(0), "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	collectSpeedText = std::make_unique<text>(this, "Collect Speed:", "straight", vector{ 0, 0 });
	collectSpeedValue = std::make_unique<text>(this, shortNumbers::convert2Short(0), "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	
	buyButton = std::make_unique<Ubutton>(this, "widget/biggerButton.png", 31, 15, 2, vector{ 0, 0 }, false, false);
	buyButton->addCallback(this, &UmechanicWidget::upgradeFishTransporter);
	multi1x = std::make_unique<Ubutton>(this, "widget/button.png", 27, 13, 2, vector{ 0, 0 }, false, false);
	multi10x = std::make_unique<Ubutton>(this, "widget/button.png", 27, 13, 2, vector{ 0, 0 }, false, false);
	multiMax = std::make_unique<Ubutton>(this, "widget/button.png", 27, 13, 2, vector{ 0, 0 }, false, false);

	upgradePriceText = std::make_unique<text>(this, "0.00k", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	currencyIcon = std::make_unique<Image>("./images/currency/coin" + std::to_string(id + 1) + ".png", vector{ 0, 0 }, false);

	setup();
}

void UmechanicWidget::setup() {
	setupLocs();
}

UmechanicWidget::~UmechanicWidget() {

}

void UmechanicWidget::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	if (!visible)
		return;

	upgradeBackground->draw(shaderProgram);
	npcBackground->draw(shaderProgram);
	if (infoBackground)
		infoBackground->draw(shaderProgram);

	npcImg->draw(shaderProgram);

	nameHolder->draw(shaderProgram);

	// fish transporter
	fishTransporterImg->draw(shaderProgram);
	if (!saveMechanicStruct->unlocked) {
		buyFishTransporterButton->draw(shaderProgram);
		buyFishTransporterText->draw(shaderProgram);
		buyFishTransporterPriceText->draw(shaderProgram);
	} else {
		fishTransporterName->draw(shaderProgram);
		level->draw(shaderProgram);
		levelProgress->draw(shaderProgram);
		maxHoldText->draw(shaderProgram);
		maxHoldValue->draw(shaderProgram);
		speedText->draw(shaderProgram);
		speedValue->draw(shaderProgram);
		collectSpeedText->draw(shaderProgram);
		collectSpeedValue->draw(shaderProgram);
		buyButton->draw(shaderProgram);
		//buyButtonText->draw(shaderProgram);
		//multi1x->draw(shaderProgram);
		//multi10x->draw(shaderProgram);
		//multiMax->draw(shaderProgram);

		upgradePriceText->draw(shaderProgram);
		currencyIcon->draw(shaderProgram);
	}

	closeButton->draw(shaderProgram);
}

void UmechanicWidget::setNameDescription(std::string nameString, std::string descriptionString) {
	name->setText(nameString);
	description->setText(descriptionString);

	// change nameHolder sizes
	nameHolder->changeChildHeight(name.get(), name->getSize().y + stuff::pixelSize);
}

void UmechanicWidget::setupLocs() {
	__super::setupLocs();

	float x = (npcBackground->getSize().x + 1) * stuff::pixelSize;
	float y = (npcBackground->getSize().y + 1) * stuff::pixelSize;
	vector size = vector{ x, 0 } + upgradeBackground->getSize();
	vector center = { stuff::screenSize.x / 2, stuff::screenSize.y / 2 };
	vector topLeft = center - size / 2;
	npcBackground->setLoc(topLeft);
	if (infoBackground)
		infoBackground->setLoc(topLeft + vector{ 0, y });
	upgradeBackground->setLoc(topLeft + vector{x, 0});

	vector npcBackgroundSize = npcBackground->getSize();
	vector npcSize = npcImg->getSize();
	npcImg->setLoc(npcBackground->getLoc() + vector{npcBackgroundSize.x / 2, npcBackgroundSize.y} - vector{npcSize.x / 2, npcSize.y});

	if (closeButton) {
		vector closeButtonSize = closeButton->getSize();
		closeButton->setLoc({ float(upgradeBackground->getLoc().x + upgradeBackground->getSize().x * stuff::pixelSize - closeButtonSize.x / 2), float(upgradeBackground->getLoc().y - closeButtonSize.y / 2)});
	}

	if (infoBackground) {
		nameHolder->setLocAndSize({ float(infoBackground->getLoc().x) + 6 * stuff::pixelSize, float(infoBackground->getLoc().y) + 6 * stuff::pixelSize}, infoBackground->getSize() * stuff::pixelSize);
		name->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);
		description->setLineLength((infoBackground->getSize().x - 10) * stuff::pixelSize);
	}

	// bought fish transporter
	fishTransporterName->setLoc(upgradeBackground->getLoc() + vector{6, 9} * stuff::pixelSize);
	buyFishTransporterText->setLoc(fishTransporterName->getLoc());
	fishTransporterImg->setLoc(fishTransporterName->getLoc() + vector{2 * stuff::pixelSize, fishTransporterName->getSize().y + 15 * stuff::pixelSize});

	float xStart = fishTransporterImg->getLoc().x + fishTransporterImg->getSize().x;
	float xEnd = upgradeBackground->getLoc().x + upgradeBackground->getSize().x - 6 * stuff::pixelSize;
	float xMid = (xEnd - xStart) / 2.f + xStart;
	if (buyFishTransporterButton) {
		//buyFishTransporterButton->setLoc(upgradeBackground->loc + upgradeBackground->getSize() / 2 - buyFishTransporterButton->getSize() / 2);
		// find mid between fish transporter and edge of shop
		buyFishTransporterButton->setLoc(vector{ xMid - buyFishTransporterButton->getSize().x / 2.f, upgradeBackground->getLoc().y + upgradeBackground->getSize().y / 2 - buyFishTransporterButton->getSize().y / 2});
		buyFishTransporterPriceText->setLoc({ xMid, upgradeBackground->getLoc().y + upgradeBackground->getSize().y / 2});
	}

	level->setLoc(vector{ fishTransporterImg->getLoc().x + fishTransporterImg->getSize().x + 130 * stuff::pixelSize, upgradeBackground->getLoc().y + 15 * stuff::pixelSize});
	levelProgress->setLoc(level->getLoc() + vector{ -125, 9 } * stuff::pixelSize);
	maxHoldText->setLoc(levelProgress->getLoc() + vector{ 0, 23 * stuff::pixelSize });
	maxHoldValue->setLoc(vector{ upgradeBackground->getLoc().x + upgradeBackground->getSize().x - 5 * stuff::pixelSize, maxHoldText->getLoc().y});
	speedText->setLoc(maxHoldText->getLoc() + vector{ 0, 23 * stuff::pixelSize });
	speedValue->setLoc(maxHoldValue->getLoc() + vector{ 0, 20 * stuff::pixelSize });
	collectSpeedText->setLoc(speedText->getLoc() + vector{ 0, 23 * stuff::pixelSize });
	collectSpeedValue->setLoc(speedValue->getLoc() + vector{ 0, 20 * stuff::pixelSize });
	if (buyButton) {
		buyButton->setLoc({ xMid - buyButton->getSize().x / 2.f, collectSpeedText->getLoc().y + 20 * stuff::pixelSize });
		//buyButtonText->setLoc(buyButton->getLoc() + buyButton->getSize() / 2 - buyButtonText->getSize() / 2);
		multiMax->setLoc(buyButton->getLoc() - vector{ buyButton->getSize().x + 3 * stuff::pixelSize, 0 });
		multi10x->setLoc(multiMax->getLoc() - vector{ multiMax->getSize().x, 0 });
		multi1x->setLoc(multi10x->getLoc() - vector{ multi10x->getSize().x, 0 });
		upgradePriceText->setLoc(buyButton->getLoc() + buyButton->getSize() / 2.f);
		currencyIcon->setLoc(buyButton->getLoc() + vector{ -currencyIcon->getSize().x - stuff::pixelSize * 2.f, buyButton->getSize().y / 2.f - currencyIcon->getSize().y / 2.f});
	}
}

void UmechanicWidget::buyFishTransporter() {
	int id = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	if (SaveData::saveData.currencyList[id+1].numOwned >= mechanicStruct->currencyNum) {
		SaveData::saveData.currencyList[id+1].numOwned -= mechanicStruct->currencyNum;
		saveMechanicStruct->unlocked = true;
		Main::currencyWidget->updateList();

		// spawn the fish transproter
		std::cout << "spawning the fish transporter!" << std::endl;
		world::currWorld->spawnFishTransporter();
		fishTransporterImg->setColorMod(glm::vec4(1));

		// update text
	}

	update();
}

void UmechanicWidget::update() {
	level->setText(std::to_string(saveMechanicStruct->level) + "/100");
	levelProgress->setPercent(saveMechanicStruct->level / 100.f);

	upgradePriceText->setText(shortNumbers::convert2Short(calcUpgradeCost()));

	Main::currencyWidget->updateList();

	if (world::currWorld->fishTransporter) {
		maxHoldValue->setText(shortNumbers::convert2Short(world::currWorld->fishTransporter->getMaxHoldNum()));
		speedValue->setText(shortNumbers::convert2Short(world::currWorld->fishTransporter->getSpeed()));
		collectSpeedValue->setText(shortNumbers::convert2Short(world::currWorld->fishTransporter->getCollectionSpeed(), true));
	}
}

void UmechanicWidget::upgradeFishTransporter() {
	FsaveCurrencyStruct& currencyStruct = SaveData::saveData.currencyList[Scene::getWorldIndexFromName(Scene::getCurrWorldName())+1];
	double cost = calcUpgradeCost();
	if (saveMechanicStruct->level < 100 && currencyStruct.numOwned >= cost) {
		currencyStruct.numOwned -= cost;
		if (world::currWorld->fishTransporter)
			world::currWorld->fishTransporter->upgrade(saveMechanicStruct);
		update();
	}
}

double UmechanicWidget::calcUpgradeCost() {
	return saveMechanicStruct->level * 100;
}