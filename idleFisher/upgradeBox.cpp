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
#include "achievement.h"

#include "debugger.h"

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, SaveEntry* saveWorldStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->saveWorldStruct = saveWorldStruct;

	callback = std::bind(&UupgradeBox::openWorld, this);

	setup(saveWorldStruct->id);
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, ModifierNode* data, SaveEntry* saveData, UpgradeBoxType type) : widget(parent) {
	this->NPCWidget = NPCWidget;

	if (type == UpgradeBoxType::Upgrade) {
		upgradeStruct = data;
		saveUpgradeStruct = saveData;

		setup(upgradeStruct->id);
	} else if (type == UpgradeBoxType::Pet){ // is pet
		petStruct = data;
		savePetStruct = saveData;

		callback = std::bind(&UupgradeBox::spawnPet, this);
		thumbnail = std::make_unique<Image>("images/pets/pet" + std::to_string(savePetStruct->id) + ".png", vector{ 0, 0 }, false);
	
		setup(savePetStruct->id);
	} else if (type == UpgradeBoxType::Bait) {
		this->NPCWidget = NPCWidget;
		this->baitStruct = data;
		this->saveBaitStruct = saveData;

		callback = std::bind(&UupgradeBox::equipBait, this);

		thumbnail = std::make_unique<Image>("images/widget/thumbnails/bait" + std::to_string(baitStruct->id) + ".png", vector{ 0, 0 }, false);

		setup(baitStruct->id);
	}
}

UupgradeBox::UupgradeBox(widget* parent, widget* NPCWidget, FvaultUnlocksStruct* vaultUnlocksStruct, SaveEntry* saveVaultUnlocksStruct) : widget(parent) {
	this->NPCWidget = NPCWidget;
	this->vaultUnlocksStruct = vaultUnlocksStruct;
	this->saveVaultUnlocksStruct = saveVaultUnlocksStruct;

	setup(vaultUnlocksStruct->id);
}

UupgradeBox::~UupgradeBox() {

}

void UupgradeBox::setup(uint32_t progressId) {
	progressNode = &SaveData::data.progressionData.at(progressId);
	saveProgressNode = &SaveData::saveData.progressionData.at(progressId);

	nameString = progressNode->name;
	descriptionString = progressNode->description;

	background = std::make_unique<Image>("images/widget/upgradeBoxBackground.png", vector{ 0, 0 }, false);

	thumbnailBackground = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	thumbnailBackground->SetPivot({ 0.f, 0.5f });
	name = std::make_unique<text>(this, nameString, "straightDark", vector{ 0, 0 });
	name->SetPivot({ 0.f, 0.5f });

	buyButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 2, vector{0, 0}, false, false);
	buyButton->addCallback(this, &UupgradeBox::buyUpgrade);
	buyButton->SetPivot({ 1.f, 0.5f });

	buttonPriceText = std::make_unique<text>(this, "0", "straightDark", vector{0, 0}, false, false, TEXT_ALIGN_CENTER);
	buttonPriceText->setTextColor(glm::vec4(1, 0, 0, 1));
	buttonPriceText->SetPivot({ 0.f, 0.5f });

	currencyImg = std::make_unique<Image>("images/currency/coin" + std::to_string(progressNode->worldId) + ".png", vector{ 0, 0 }, false);
	currencyImg->SetPivot({ 1.f, 0.5f });

	if (progressNode->maxLevel > 1) {
		upgradeText = std::make_unique<text>(this, std::to_string(saveProgressNode->level) + "/" + std::to_string(progressNode->maxLevel), "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_RIGHT);
		upgradeText->SetPivot({ 0.f, 0.5f });
	}

	update();
}

void UupgradeBox::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	background->draw(shaderProgram);

	thumbnailBackground->draw(shaderProgram);
	if (thumbnail)
		thumbnail->draw(shaderProgram);
	name->draw(shaderProgram);
	buyButton->draw(shaderProgram);

	double price = Upgrades::GetPrice(progressNode->id);
	if (price <= SaveData::saveData.currencyList.at(progressNode->worldId).numOwned || (saveProgressNode->level >= progressNode->maxLevel)) // can afford, or max level
		buttonPriceText->setTextColor(glm::vec4(1)); // set to og color
	else // cant afford
		buttonPriceText->setTextColor(glm::vec4(1, 0, 0, 1)); // set red

	buttonPriceText->draw(shaderProgram);

	// draw if not max or unlocked
	if (saveProgressNode->level < progressNode->maxLevel) // if not max level
		currencyImg->draw(shaderProgram);

	if (upgradeText)
		upgradeText->draw(shaderProgram);
	
	if (mouseOver()) { // changes the text only if it isn't already set to current world
		if (NPCwidget* widget = dynamic_cast<NPCwidget*>(NPCWidget)) {
			if (widget->name->getString() != nameString)
				widget->setNameDescription(nameString, descriptionString);
		} else if (UmerchantWidget* widget = dynamic_cast<UmerchantWidget*>(NPCWidget)) {
			if (widget && widget->name->getString() != nameString)
				widget->setNameDescription(nameString, descriptionString);
		} else if (UfishermanWidget* widget = dynamic_cast<UfishermanWidget*>(NPCWidget)) {
			if (widget && widget->name->getString() != nameString) {
				widget->setNameDescription(nameString, descriptionString);
			}
		}
	}
}

void UupgradeBox::setLocAndSize(vector loc, vector size) {
	__super::setLocAndSize(loc, size);
	setupLocs();
}

void UupgradeBox::setupLocs() {
	if (background)
		background->setLoc(absoluteLoc);

	if (buyButton) {
		vector buyButtonLoc = (absoluteLoc + vector{ size.x - 3.f, size.y / 2.f });
		buyButton->setLoc(buyButtonLoc);

		if (buttonPriceText)
			buttonPriceText->setLoc(buyButtonLoc - vector{ buyButton->getSize().x / 2.f, 0.f });

		if (currencyImg)
			currencyImg->setLoc(buyButtonLoc - vector{ buyButton->getSize().x + 3.f, 0.f });

		if (upgradeText) {
			vector upgradeTextLoc = vector{ currencyImg->getAbsoluteLoc().x - 3.f, absoluteLoc.y + size.y / 2.f };
			upgradeText->setLoc(upgradeTextLoc);
		}
	}

	if (thumbnailBackground)
		thumbnailBackground->setLoc((loc + vector{ 4.f, size.y / 2.f }));
	
	if (thumbnail && thumbnailBackground)
		thumbnail->setLoc(thumbnailBackground->getAbsoluteLoc());
	
	if (name)
		name->setLoc((absoluteLoc + vector{ (thumbnailBackground->getSize().x + 6.f), size.y / 2.f }));
}

bool UupgradeBox::mouseOver() {
	// parent assumed to be vertical box
	vector parentLoc = getParent() ? getParent()->getOgLoc() : vector{ 0, 0 };
	vector parentSize = getParent() ? getParent()->getSize() : vector{ 0, 0 };

	vector mousePos = Input::getMousePos();
	bool mouseInVertBox = mousePos.x >= parentLoc.x && mousePos.x <= parentLoc.x + parentSize.x && mousePos.y >= parentLoc.y && mousePos.y <= parentLoc.y + parentSize.y;
	return mouseInVertBox && background->isMouseOver();
}

void UupgradeBox::buyUpgrade() {
	if (saveProgressNode->level < progressNode->maxLevel && !Upgrades::LevelUp(progressNode->id)) // if not max level, and don't have enough currency
		return; // didn't have enough money to purchase upgrade

	Achievements::CheckGroup(AchievementTrigger::CurrencyPerSecond);

	// will equip the item or object once the layer has unlocked it, instead of needing to click twice
	if (savePetStruct && !savePetStruct->level)
		spawnPet();
	else if (saveBaitStruct && !saveBaitStruct->level)
		equipBait();

	if (saveProgressNode->level >= progressNode->maxLevel) { // if max level / unlocked
		if (callback)
			callback();

		if (petStruct) {
			Achievements::CheckGroup(AchievementTrigger::PetPurchased);

			NPCwidget* npcWidget = dynamic_cast<NPCwidget*>(NPCWidget);
			if (npcWidget)
				for (int i = 0; i < npcWidget->upgradeHolder->GetChildrenCount(); i++) {
					widget* child = npcWidget->upgradeHolder->GetChildAt(i).child;
					UupgradeBox* upgradeBox = dynamic_cast<UupgradeBox*>(child);
					if (upgradeBox && upgradeBox->savePetStruct->level)
						upgradeBox->buttonPriceText->setText("equip");
				}
			buttonPriceText->setText("remove");
		} else if (baitStruct) {
			Achievements::CheckGroup(AchievementTrigger::BaitPurchased);

			UfishermanWidget* fishermanWidget = dynamic_cast<UfishermanWidget*>(NPCWidget);
			if (fishermanWidget) {
				for (int i = 0; i < fishermanWidget->baitHolderList->GetChildrenCount(); i++) {
					widget* child = fishermanWidget->baitHolderList->GetChildAt(i).child;
					UupgradeBox* upgradeBox = dynamic_cast<UupgradeBox*>(child);
					if (upgradeBox && upgradeBox->saveBaitStruct->level)
						upgradeBox->buttonPriceText->setText("equip");
				}
			}
			buttonPriceText->setText("remove");
		}
	}

	update();
	Main::heldFishWidget->updateList(true); // update held fish widget, incase something like an upgrade affects it
}

void UupgradeBox::update() {
	if (upgradeText)
		upgradeText->setText(std::to_string(saveProgressNode->level) + "/" + std::to_string(progressNode->maxLevel));

	// show remove or equipped if unlocked
	if (saveProgressNode->level >= progressNode->maxLevel) {
		if (petStruct) { // if unlocked
			if (SaveData::saveData.equippedPetId == petStruct->id)
				buttonPriceText->setText("remove");
			else
				buttonPriceText->setText("equip");
		} else if (baitStruct) { // if unlocked
			if (SaveData::saveData.equippedBaitId == baitStruct->id) {
				buttonPriceText->setText("remove");
			} else if (saveBaitStruct->level)
				buttonPriceText->setText("equip");
		} else { // if maxed
			buyButton->enable(false);
			buttonPriceText->setText("Max");
		}
	} else {
		double price = Upgrades::GetPrice(progressNode->id);
		buttonPriceText->setText(shortNumbers::convert2Short(price));
	}
}

void UupgradeBox::openWorld() {
	Scene::openLevel(saveWorldStruct->id);
}

void UupgradeBox::spawnPet() {
	// remove already existing pet
	// set pet
	if (!Scene::pet.get() || (Scene::pet && petStruct->id != Scene::pet->getPetStruct()->id)) {
		SaveData::saveData.equippedPetId = petStruct->id;
		Scene::pet = std::make_unique<Apet>(petStruct, vector{ 400, -200 });
	} else if (Scene::pet && petStruct->id == Scene::pet->getPetStruct()->id) {
		Scene::pet.reset();
		SaveData::saveData.equippedPetId = 0;
	}

	Main::heldFishWidget->updateList(true);
}

void UupgradeBox::equipBait() {
	if (SaveData::saveData.equippedBaitId == 0 || baitStruct->id != SaveData::saveData.equippedBaitId)
		GetCharacter()->equipBait(baitStruct->id);
	else {
		// unequip bait
		SaveData::saveData.equippedBaitId = 0;
	}
}

vector UupgradeBox::getSize() {
	if (background)
		return background->getSize();
	return { 0, 0 };
}