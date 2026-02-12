#include "equippedWidget.h"
#include "main.h"
#include "saveData.h"
#include "hoverBox.h"
#include "button.h"
#include "text.h"
#include "achievementWidget.h"
#include "upgrades.h"
#include "shortNumbers.h"

#include "debugger.h"

UequippedWidget::UequippedWidget(widget* parent) : widget(parent) {
	//charImg = std::make_unique<Image>("images/character/idleSE1.png", vector{ 0, 0 }, false);

	fishingRodEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	baitEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	petEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);

	//headEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	//shirtEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	//legsEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);
	//shoesEquipSlot = std::make_unique<Image>("images/widget/thumbnailBackground.png", vector{ 0, 0 }, false);

	update();
}

UequippedWidget::~UequippedWidget() {

}

void UequippedWidget::draw(Shader* shaderProgram) {
	//charImg->draw(shaderProgram);
	fishingRodEquipSlot->draw(shaderProgram);
	baitEquipSlot->draw(shaderProgram);
	petEquipSlot->draw(shaderProgram);

	//headEquipSlot->draw(shaderProgram);
	//shirtEquipSlot->draw(shaderProgram);
	//legsEquipSlot->draw(shaderProgram);
	//shoesEquipSlot->draw(shaderProgram);

	if (fishingRodEquip)
		fishingRodEquip->draw(shaderProgram);
	if (baitEquip)
		baitEquip->draw(shaderProgram);
	if (petEquip)
		petEquip->draw(shaderProgram);
}

void UequippedWidget::update() {
	fishingRodEquip = std::make_unique<Image>("images/widget/thumbnails/fishingRodThumbnail.png", vector{0, 0}, false);
	fishingRodEquip->setLoc(fishingRodEquipSlot->getLoc());

	std::string thumbnail = "";
	if (SaveData::saveData.equippedBaitId != 0u)
		thumbnail = "images/widget/thumbnails/bait" + std::to_string(SaveData::saveData.equippedBaitId) + ".png";
	baitEquip = std::make_unique<Image>(thumbnail, vector{ 0, 0 }, false);
	baitEquip->setLoc(baitEquipSlot->getLoc());

	
	std::string petThumbnail = "";
	if (SaveData::saveData.equippedPetId != 0u)
		petThumbnail = "images/pets/pet" + std::to_string(SaveData::saveData.equippedPetId) + ".png";
	petEquip = std::make_unique<Image>(petThumbnail, vector{0, 0}, false);
	petEquip->setLoc(petEquipSlot->getLoc());
}

vector UequippedWidget::getSize() {
	vector size = fishingRodEquipSlot->getSize();
	return vector{ size.x * 2, size.y };
}

void UequippedWidget::setLoc(vector loc) {
	__super::setLoc(loc);

	vector parentSize = getParent()->getSize();
	vector center = loc + vector((parentSize.x - fishingRodEquipSlot->getSize().x) / 2, 0);

	vector size = fishingRodEquipSlot->getSize();

	//charImg->setLoc(offset + getSize() / 2 - charImg->getSize() / 2);

	fishingRodEquipSlot->setLoc(center - vector(size.x, 0.f));
	baitEquipSlot->setLoc(center + vector(0.f, 0.f));
	petEquipSlot->setLoc(center + vector(size.x, 0.f));

	//headEquipSlot->setLoc(offset + vector{ 0, size.y });
	//shirtEquipSlot->setLoc(offset + vector{ 0, size.y * 2 });
	//legsEquipSlot->setLoc(offset + vector{ size.x * 3, size.y });
	//shoesEquipSlot->setLoc(offset + vector{ size.x * 3, size.y * 2 });

	if (fishingRodEquip)
		fishingRodEquip->setLoc(fishingRodEquipSlot->getLoc());
	if (baitEquip)
		baitEquip->setLoc(baitEquipSlot->getLoc());
	if (petEquip)
		petEquip->setLoc(petEquipSlot->getLoc());
	/*if (headEquip)
		headEquip->setLoc(headEquipSlot->getLoc());
	if (shirtEquip)
		shirtEquip->setLoc(shirtEquipSlot->getLoc());
	if (legsEquip)
		legsEquip->setLoc(legsEquipSlot->getLoc());
	if (shoesEquip)
		shoesEquip->setLoc(shoesEquipSlot->getLoc());*/
}

bool UequippedWidget::mouseOverSlot(std::string &name, std::string &description) {
	bool hovering = false;
	if (fishingRodEquip && fishingRodEquip->isMouseOver()) {
		name = "Fishing Rod";
		std::string power = shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::Power));
		std::string speed = shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::FishComboSpeed));
		std::string catchChance = shortNumbers::convert2Short(Upgrades::GetBaseStat(Stat::CatchNum));
		description = "Power: " + power + "\nSpeed: " + speed + "\nChance: " + catchChance;
		hovering = true;
	} else if (SaveData::saveData.equippedBaitId != 0u && baitEquip && baitEquip->isMouseOver()) {
		ProgressionNode& baitProgNode = SaveData::data.progressionData.at(SaveData::saveData.equippedBaitId);
		name = baitProgNode.name;
		description = baitProgNode.description;
		hovering = true;
	} else if (SaveData::saveData.equippedPetId != 0u && petEquip && petEquip->isMouseOver()) {
		ProgressionNode& petProgNode = SaveData::data.progressionData.at(SaveData::saveData.equippedPetId);
		name = petProgNode.name;
		description = petProgNode.description;
		hovering = true;
	} /*else if (headEquip && headEquip->isMouseOver()) {
		hovering = true;
	} else if (shirtEquip && shirtEquip->isMouseOver()) {
		hovering = true;
	} else if (legsEquip && legsEquip->isMouseOver()) {
		hovering = true;
	} else if (shoesEquip && shoesEquip->isMouseOver()) {
		hovering = true;
		
	}*/

	if (hovering)
		return true;
	return false;
}