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
	closeButton->SetPivot({ 0.5f, 0.5f });
	closeButton->addCallback<widget>(this, &UmechanicWidget::removeFromViewport);

	npcImg = std::make_unique<Image>("images/widget/npcButtons/mechanic.png", vector{ 0, 0 }, false);
	npcImg->SetPivot({ 0.5f, 0.f });

	name = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0,0 });
	description = std::make_unique<text>(this, " ", "straight", vector{ 0,0 });
	nameHolder = std::make_unique<verticalBox>(this);
	nameHolder->addChild(name.get(), 8.f);
	nameHolder->addChild(description.get(), 4.f);

	npcBackground = std::make_unique<Image>("images/widget/npcBackground.png", vector{ 0, 0 }, false);
	npcBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	npcBackground->SetPivot({ 1.f, 0.f });
	infoBackground = std::make_unique<Image>("images/widget/infoBackground.png", vector{ 0, 0 }, false);
	infoBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	infoBackground->SetPivot({ 1.f, 1.f });
	upgradeBackground = std::make_unique<Image>("images/widget/upgradeBackground.png", vector{ 0, 0 }, false);
	upgradeBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	upgradeBackground->SetPivot({ 0.f, 0.5f });

	// fish transporter
	buyFishTransporterButton = std::make_unique<Ubutton>(this, "widget/button.png", 27, 13, 2, vector{ 0, 0 }, false, false);
	buyFishTransporterButton->SetPivot({ 0.5f, 0.5f });
	buyFishTransporterButton->addCallback(this, &UmechanicWidget::buyFishTransporter);
	buyFishTransporterText = std::make_unique<text>(this, "Buy Fish Transporter", "biggerStraight", vector{ 0, 0 });
	buyFishTransporterPriceText = std::make_unique<text>(this, shortNumbers::convert2Short(mechanicStruct->currencyNum), "straight", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);
	buyFishTransporterPriceText->SetPivot({ 0.f, 0.5f });

	// bought screen
	fishTransporterName = std::make_unique<text>(this, "Fish Transporter", "biggerStraight", vector{ 0, 0 });
	fishTransporterImg = std::make_unique<Image>("images/npc/fishTransporter/idleSE.png", vector{ 0, 0 }, false);
	
	if (saveMechanicStruct->unlocked)
		fishTransporterImg->setColorMod(glm::vec4(1));
	else
		fishTransporterImg->setColorMod(glm::vec4(glm::vec3(0), 1.f));

	fishTransporterImg->setSize(fishTransporterImg->getSize() * 2.5f);
	fishTransporterImg->SetPivot({ 0.f, 1.f });
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
	upgradePriceText->SetPivot({ 0.f, 0.5f });
	currencyIcon = std::make_unique<Image>("images/currency/coin" + std::to_string(id + 1) + ".png", vector{ 0, 0 }, false);
	currencyIcon->SetPivot({ 1.f, 0.5f });

	setup();
	update();
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

	CheckTextColor();

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

	float widgetWidth = npcBackground->getSize().x + upgradeBackground->getSize().x;
	vector center = vector{ widgetWidth / 2.f - upgradeBackground->getSize().x, 0.f };
	npcBackground->setLoc(center + vector{ -1.f, 1.f });
	infoBackground->setLoc(center + vector{ -1.f, -1.f });
	upgradeBackground->setLoc(center + vector{ 1.f, 1.f });

	if (npcImg)
		npcImg->setLoc(npcBackground->getAbsoluteLoc() + vector{ npcBackground->getSize().x / 2.f, 0.f });

	if (closeButton)
		closeButton->setLoc(upgradeBackground->getAbsoluteLoc() + upgradeBackground->getSize());

	vector nameHolderSize = infoBackground->getSize() - 10.f;
	name->setLineLength(nameHolderSize.x);
	description->setLineLength(nameHolderSize.x);
	nameHolder->setLocAndSize(infoBackground->getAbsoluteLoc() + 5.f, nameHolderSize);

	// bought fish transporter
	fishTransporterName->setLoc(upgradeBackground->getAbsoluteLoc() + vector{ 6.f, upgradeBackground->getSize().y - 13.f });
	buyFishTransporterText->setLoc(fishTransporterName->getAbsoluteLoc());
	fishTransporterImg->setLoc(fishTransporterName->getAbsoluteLoc() + vector{ 2.f, -15.f });

	if (buyFishTransporterButton) {
		buyFishTransporterButton->setLoc(upgradeBackground->getAbsoluteLoc() + upgradeBackground->getSize() / 2.f + vector{ 15.f, 0.f });
		buyFishTransporterPriceText->setLoc(buyFishTransporterButton->getAbsoluteLoc() + buyFishTransporterButton->getSize() / 2.f);
	}

	level->setLoc(fishTransporterName->getAbsoluteLoc() + vector{ fishTransporterName->getSize().x + 92.f, -20.f });
	levelProgress->setLoc(level->getAbsoluteLoc() + vector{ -125.f, -9.f });
	maxHoldText->setLoc(levelProgress->getAbsoluteLoc() + vector{ 0.f, -23.f });
	maxHoldValue->setLoc(vector{ upgradeBackground->getAbsoluteLoc().x + upgradeBackground->getSize().x - 5.f, maxHoldText->getAbsoluteLoc().y});
	speedText->setLoc(maxHoldText->getAbsoluteLoc() + vector{ 0.f, -23.f });
	speedValue->setLoc(maxHoldValue->getAbsoluteLoc() + vector{ 0.f, -20.f });
	collectSpeedText->setLoc(speedText->getAbsoluteLoc() + vector{ 0.f, -23.f });
	collectSpeedValue->setLoc(speedValue->getAbsoluteLoc() + vector{ 0.f, -20.f });
	if (buyButton) {
		buyButton->setLoc(collectSpeedText->getAbsoluteLoc() + vector{ 50.f, -30.f });
		multiMax->setLoc(buyButton->getAbsoluteLoc() - vector{ buyButton->getSize().x + 3.f, 0 });
		multi10x->setLoc(multiMax->getAbsoluteLoc() - vector{ multiMax->getSize().x, 0 });
		multi1x->setLoc(multi10x->getAbsoluteLoc() - vector{ multi10x->getSize().x, 0 });
		upgradePriceText->setLoc(buyButton->getAbsoluteLoc() + buyButton->getSize() / 2.f);
		currencyIcon->setLoc(buyButton->getAbsoluteLoc() + vector{ -4.f, buyButton->getSize().y / 2.f });
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

	upgradeCost = calcUpgradeCost();
	if (saveMechanicStruct->level < 100)
		upgradePriceText->setText(shortNumbers::convert2Short(upgradeCost));
	else {
		upgradePriceText->setText("Max");
		buyButton->enable(false);
	}

	Main::currencyWidget->updateList();

	AfishTransporter* fishTransporter = world::GetFishTransporter();
	if (fishTransporter) {
		maxHoldValue->setText(shortNumbers::convert2Short(fishTransporter->getMaxHoldNum()));
		speedValue->setText(shortNumbers::convert2Short(fishTransporter->getSpeed()));
		collectSpeedValue->setText(shortNumbers::convert2Short(fishTransporter->getCollectionSpeed(), true));
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
	return saveMechanicStruct->level * 100 + 100;
}

void UmechanicWidget::CheckTextColor() {
	// power
	if (SaveData::saveData.currencyList[1].numOwned >= mechanicStruct->currencyNum)
		buyFishTransporterPriceText->setTextColor(255, 255, 255);
	else
		buyFishTransporterPriceText->setTextColor(255, 0, 0);

	// speed
	if (SaveData::saveData.currencyList[1].numOwned >= upgradeCost || saveMechanicStruct->level >= 100)
		upgradePriceText->setTextColor(255, 255, 255);
	else
		upgradePriceText->setTextColor(255, 0, 0);
}