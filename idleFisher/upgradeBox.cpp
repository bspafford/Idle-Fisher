#include "upgradeBox.h"

#include "NPCwidget.h"
#include "main.h"
#include "Input.h"
#include "character.h"
#include "saveData.h"
#include "pet.h"
#include "upgrades.h"
#include "shortNumbers.h"
#include "animation.h"
#include "text.h"
#include "button.h"
#include "verticalBox.h"
#include "merchantWidget.h"
#include "fishermanWidget.h"

#include "debugger.h"

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FworldStruct* worldStruct, FsaveWorldStruct* saveWorldStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->worldStruct = worldStruct;
	this->saveWorldStruct = saveWorldStruct;

	nameString = worldStruct->name;
	descriptionString = worldStruct->description;

	unlocked = &saveWorldStruct->unlocked;
	price = &worldStruct->currencyNum;
	currencyId = &worldStruct->currencyId;

	callback = std::bind(&UupgradeBox::openWorld, this);

	setup();
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FbaitStruct* baitStruct, FsaveBaitStruct* saveBaitStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->baitStruct = baitStruct;
	this->saveBaitStruct = saveBaitStruct;

	nameString = baitStruct->name;
	descriptionString = baitStruct->description;
	buffString = baitStruct->buffs;
	debuffString = baitStruct->debuffs;

	unlocked = &saveBaitStruct->unlocked;
	price = &baitStruct->currencyNum;
	currencyId = &baitStruct->currencyId;

	callback = std::bind(&UupgradeBox::equipBait, this);

	thumbnail = std::make_unique<Image>(baitStruct->thumbnail, vector{ 0, 0 }, false);
	
	setup();
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FpetStruct* petStruct, FsavePetStruct* savePetStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->petStruct = petStruct;
	this->savePetStruct = savePetStruct;

	nameString = petStruct->name;
	descriptionString = petStruct->description;

	unlocked = &savePetStruct->unlocked;
	price = &petStruct->currencyNum;
	currencyId = &petStruct->currencyId;

	callback = std::bind(&UupgradeBox::spawnPet, this);

	thumbnail = std::make_unique<Image>("./images/pets/" + std::to_string(petStruct->id) + ".png", vector{ 0, 0 }, false);

	setup();
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FupgradeStruct* upgradeStruct, FsaveUpgradeStruct* saveUpgradeStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->upgradeStruct = upgradeStruct;
	this->saveUpgradeStruct = saveUpgradeStruct;

	nameString = upgradeStruct->name;
	descriptionString = upgradeStruct->description;

	upgradeNum = &saveUpgradeStruct->upgradeLevel;
	upgradeMax = upgradeStruct->upgradeNumMax;
	priceFallback = upgrades::calcPrice(upgradeStruct, saveUpgradeStruct);
	price = &priceFallback;
	currencyId = &upgradeStruct->currencyId;

	setup();
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FvaultUnlocksStruct* vaultUnlocksStruct, FsaveVaultUnlocksStruct* saveVaultUnlocksStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->vaultUnlocksStruct = vaultUnlocksStruct;
	this->saveVaultUnlocksStruct = saveVaultUnlocksStruct;

	nameString = vaultUnlocksStruct->name;
	descriptionString = vaultUnlocksStruct->description;

	unlocked = &saveVaultUnlocksStruct->unlocked;
	price = &vaultUnlocksStruct->currencyNum;
	currencyId = &vaultUnlocksStruct->currencyId;

	setup();
}

UupgradeBox::~UupgradeBox() {

}

void UupgradeBox::setup() {
	background = std::make_unique<Image>("./images/widget/upgradeBoxBackground.png", vector{ 0, 0 }, false);

	thumbnailBackground = std::make_unique<Image>("./images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	name = std::make_unique<text>(this, nameString, "straightDark", vector{ 0, 0 });

	buyButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{0, 0}, false, false);
	buyButton->addCallback(this, &UupgradeBox::buyUpgrade);

	buttonPriceText = std::make_unique<text>(this, shortNumbers::convert2Short(*price), "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	buttonPriceText->setTextColor(255, 0, 0);

	currencyImg = std::make_unique<Image>("./images/currency/coin" + std::to_string(*currencyId) + ".png", vector{ 0, 0 }, false);

	if (upgradeMax > 1) {
		upgradeText = std::make_unique<text>(this, std::to_string(*upgradeNum) + "/" + std::to_string(upgradeMax), "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
	}

	if ((upgradeNum && *upgradeNum >= upgradeMax) || (unlocked && *unlocked)) {
		if (petStruct) {// compare to see what save pet
			if (SaveData::saveData.equippedPet.id == petStruct->id)
				buttonPriceText->setText("remove");
			else
				buttonPriceText->setText("equip");
		} else if (baitStruct) {
			if (SaveData::saveData.equippedBait.id == baitStruct->id) {
				buttonPriceText->setText("remove");
			} else if (saveBaitStruct->unlocked)
				buttonPriceText->setText("equip");
		} else {
			buyButton->enable(false);
		}
	}
}

void UupgradeBox::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	background->draw(shaderProgram);

	thumbnailBackground->draw(shaderProgram);
	if (thumbnail)
		thumbnail->draw(shaderProgram);
	name->draw(shaderProgram);
	buyButton->draw(shaderProgram);

	if (upgradeStruct) {
		//int worldId = upgrades::worldNameToId(upgradeStruct->levelName);
		//if (*price <= SaveData::saveData.currencyList[worldId + 1].numOwned) // can afford
		if (*price <= SaveData::saveData.currencyList[*currencyId].numOwned) // can afford
			buttonPriceText->setTextColor(255, 255, 255); // set to og color
		else // cant afford
			buttonPriceText->setTextColor(255, 0, 0); // set red
	} else if (price) {
		//int worldId = upgrades::worldNameToId(Main::currWorld); // temp
		//if ((unlocked && *unlocked) || *price <= SaveData::saveData.currencyList[worldId + 1].numOwned) { // can afford
		if ((unlocked && *unlocked) || *price <= SaveData::saveData.currencyList[*currencyId].numOwned) { // can afford
			buttonPriceText->setTextColor(255, 255, 255); // set to og color
		} else // cant afford
			buttonPriceText->setTextColor(255, 0, 0); // set red
	}

	buttonPriceText->draw(shaderProgram);
	// draw if not max or unlocked
	if ((!unlocked || (unlocked && !*unlocked)) && (!upgradeNum || (upgradeNum && *upgradeNum < upgradeMax)))
		currencyImg->draw(shaderProgram);

	if (upgradeText)
		upgradeText->draw(shaderProgram);
	// changes the text only if it isn't already set to current world
	if (mouseOver()) {
		// disable scissor test cause its inside of scroll box and will get culled otherwise
		glDisable(GL_SCISSOR_TEST);
		if (NPCwidget* widget = dynamic_cast<NPCwidget*>(NPCWidget)) {
			if (widget->name->getString() != nameString)
				widget->setNameDescription(nameString, descriptionString);
		} else if (UmerchantWidget* widget = dynamic_cast<UmerchantWidget*>(NPCWidget)) {
			if (widget && widget->name->getString() != nameString)
				widget->setNameDescription(nameString, descriptionString);
		} else if (UfishermanWidget* widget = dynamic_cast<UfishermanWidget*>(NPCWidget)) {
			if (widget && widget->name->getString() != nameString) {
				widget->setNameDescription(nameString, buffString, debuffString);
			}
		}
		glEnable(GL_SCISSOR_TEST);
	}
}

void UupgradeBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc, size);
	setupLocs();
}

void UupgradeBox::setupLocs() {
	if (background)
		background->setLoc(loc);

	if (buyButton) {
		vector buttonSize = buyButton->getSize();
		vector buyButtonLoc = (loc + vector{ size.x - buttonSize.x, size.y / 2 - buttonSize.y / 2 } + vector{ -10 * stuff::pixelSize, 0 });
		buyButton->setLoc(buyButtonLoc);

		if (buttonPriceText) {
			buttonPriceText->setLoc(buyButtonLoc + buyButton->getSize() / 2);
			if (currencyImg)
				currencyImg->setLoc((buttonPriceText->getLoc() + vector{ -currencyImg->getSize().x - 20 * stuff::pixelSize, -currencyImg->getSize().y / 2 }));
		}

		if (upgradeText) {
			vector upgradeTextLoc = vector{ currencyImg->getLoc().x - 1 * stuff::pixelSize, loc.y + size.y / 2 };
			upgradeText->setLoc(upgradeTextLoc);
		}
	}

	if (thumbnailBackground)
		thumbnailBackground->setLoc((loc + vector{ 4 * stuff::pixelSize, size.y / 2 - thumbnailBackground->h / 2 * stuff::pixelSize }));
	
	if (thumbnail && thumbnailBackground)
		thumbnail->setLoc(thumbnailBackground->getLoc());
	
	if (name)
		name->setLoc((loc + vector{ (thumbnailBackground->h + 6) * stuff::pixelSize, size.y / 2 }));
}

bool UupgradeBox::mouseOver() {
	vector mousePos = Input::getMousePos();

	if (!getParent() && mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
		return true;
	else if (getParent()) {
		vector parentLoc = getParent()->getLoc();
		vector parentSize = getParent()->getSize();
		if (mousePos.x >= loc.x && mousePos.x <= loc.x + size.x && mousePos.y >= loc.y && mousePos.y <= loc.y + size.y)
			// test if inside the rect too
			if (mousePos.x >= parentLoc.x && mousePos.x <= parentLoc.x + parentSize.x && mousePos.y >= parentLoc.y && mousePos.y <= parentLoc.y + parentSize.y)
				return true;
	}
	return false;
}

void UupgradeBox::buyUpgrade() {

	if (upgradeStruct) {
		//bool success = upgrades::upgrade(upgrades::getUpgrade(upgradeStruct->upgradeFunctionName), this);
		//std::cout << "upgradeStruct->upgradeFunctionName: " << upgradeStruct->upgradeFunctionName << std::endl;
		bool success = upgrades::upgrade(SaveData::data.upgradeData[upgrades::getUpgrade(upgradeStruct->upgradeFunctionName)->id], this, price);
		//*price = upgrades::calcPrice(upgradeStruct, saveUpgradeStruct);
	} else { // temp
		//int worldId = upgrades::worldNameToId(Main::currWorld); // temp
		// returns if the player doesn't have enough money
		if (price)
			std::cout << "price: " << *price << " > " << SaveData::saveData.currencyList[*currencyId].numOwned << std::endl;
		if ((!unlocked || !*unlocked) && (!price || *price > SaveData::saveData.currencyList[*currencyId].numOwned)) {
		//if (!unlocked || *unlocked || !price || *price > SaveData::saveData.currencyList[worldId + 1].numOwned) {
			std::cout << "returning" << std::endl;
			return;
		}
		
		if (upgradeNum) {
			if (*upgradeNum >= upgradeMax)
				return;

			(*upgradeNum)++;
				SaveData::saveData.currencyList[*currencyId].numOwned -= *price;

			if (upgradeText)
				upgradeText->setText(std::to_string(*upgradeNum) + "/" + std::to_string(upgradeMax));
		}

		// will equip the item or object once the layer has unlocked it, instead of needing to click twice
		if (savePetStruct && !savePetStruct->unlocked)
			spawnPet();
		else if (saveBaitStruct && !saveBaitStruct->unlocked)
			equipBait();

		if ((upgradeNum && *upgradeNum >= upgradeMax) || (unlocked)) {
			if (unlocked) {
				if (!*unlocked) {
					*unlocked = true;
					SaveData::saveData.currencyList[*currencyId].numOwned -= *price;
				} else if (callback) {
					// equip / use
					callback();
				}
			}

			if (petStruct) {
				// since happening after function its gotta be reversed
				if (Main::pet.get() && petStruct == Main::pet->getPetStruct()) {
					NPCwidget* npcWidget = dynamic_cast<NPCwidget*>(NPCWidget);
					if (npcWidget)
						for (int i = 0; i < npcWidget->upgradeHolder->GetChildrenCount(); i++) {
							widget* child = npcWidget->upgradeHolder->GetChildAt(i).child;
							UupgradeBox* upgradeBox = dynamic_cast<UupgradeBox*>(child);
							if (upgradeBox && upgradeBox->savePetStruct->unlocked)
								upgradeBox->buttonPriceText->setText("equip");
						}
					buttonPriceText->setText("remove");
				} else {
					buttonPriceText->setText("equip");
				}
			} else if (baitStruct) {
				// since happening after function its gotta be reversed
				if (baitStruct->id == SaveData::saveData.equippedBait.id) {
					UfishermanWidget* fishermanWidget = dynamic_cast<UfishermanWidget*>(NPCWidget);
					if (fishermanWidget) {
						for (int i = 0; i < fishermanWidget->baitHolderList->GetChildrenCount(); i++) {
							widget* child = fishermanWidget->baitHolderList->GetChildAt(i).child;
							UupgradeBox* upgradeBox = dynamic_cast<UupgradeBox*>(child);
							if (upgradeBox && upgradeBox->saveBaitStruct->unlocked)
								upgradeBox->buttonPriceText->setText("equip");
						}
					}
					buttonPriceText->setText("remove");
					// then remove pet
				} else {

					buttonPriceText->setText("equip");
				}

			} else {
				buyButton->enable(false);
				//buyButton->removeClickAnim();
				//buyButton->setImg("./images/widget/upgradeButton3.png");
				buttonPriceText->setText("Max");
				setupLocs(); // updates the locks of the button and the upgrade text
			}
		}

		Main::currencyWidget->updateList();
	}
}

void UupgradeBox::update() {
	if (upgradeText)
		upgradeText->setText(std::to_string(*upgradeNum) + "/" + std::to_string(upgradeMax));

	if (upgradeStruct) // set the price text
		buttonPriceText->setText(shortNumbers::convert2Short(saveUpgradeStruct->price));
	else
		buttonPriceText->setText(shortNumbers::convert2Short(*price));


	if (upgradeNum && *upgradeNum >= upgradeMax) { // if max
		buyButton->enable(false);
		//buyButton->removeClickAnim();
		//buyButton->setImg("./images/widget/upgradeButton3.png");
		buttonPriceText->setText("Max");
	}

}

void UupgradeBox::openWorld() {
	Scene::openLevel(worldStruct->worldName);
}

void UupgradeBox::spawnPet() {
	// remove already existing pet
	// set pet
	if (!Main::pet.get() || Main::pet && petStruct->id != Main::pet->getPetStruct()->id) {
		Main::pet = std::make_unique<Apet>(petStruct, vector{ 400, -200 });
	} else if (Main::pet && petStruct->id == Main::pet->getPetStruct()->id) {
		Main::pet.reset();
	}

	Main::heldFishWidget->updateList();
}

void UupgradeBox::equipBait() {
	if (SaveData::saveData.equippedBait.id == -1 || baitStruct->id != SaveData::saveData.equippedBait.id)
		GetCharacter()->equipBait(baitStruct);
	else {
		// unequip bait
		SaveData::saveData.equippedBait.id = -1;
	}
}

vector UupgradeBox::getSize() {
	if (background)
		return vector{ float(background->w), float(background->h) } * stuff::pixelSize;
	return { 0, 0 };
}