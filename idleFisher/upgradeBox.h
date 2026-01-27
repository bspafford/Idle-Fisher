#pragma once

#include "widget.h"

#include <functional>

class Image;
class text;
class Ubutton;
class verticalBox;
struct FworldStruct;
struct FfishingRodStruct;
struct FbaitStruct;
struct FpetStruct;
struct FupgradeStruct;
struct FvaultUnlocksStruct;
struct SaveEntry;

class UupgradeBox : public widget {
public:
	UupgradeBox(widget* parent, widget* NPCWidget, FworldStruct* worldStruct, SaveEntry* saveWorldStruct);
	UupgradeBox(widget* parent, widget* NPCWidget, FbaitStruct* baitStruct, SaveEntry* saveBaitStruct);
	UupgradeBox(widget* parent, widget* NPCWidget, FpetStruct* data, SaveEntry* saveData);
	UupgradeBox(widget* parent, widget* NPCWidget, FupgradeStruct* data, SaveEntry* saveData);
	UupgradeBox(widget* parent, widget* NPCWidget, FvaultUnlocksStruct* data, SaveEntry* saveData);
	~UupgradeBox();
	void setup();
	void update();

	void draw(Shader* shaderProgram) override;

	vector getSize() override;

private:
	void buyUpgrade();

	widget* NPCWidget;

	// structs
	FworldStruct* worldStruct;
	SaveEntry* saveWorldStruct;
	FfishingRodStruct* fishingRodStruct;
	FbaitStruct* baitStruct;
	SaveEntry* saveBaitStruct;
	FpetStruct* petStruct;
	SaveEntry* savePetStruct;
	FupgradeStruct* upgradeStruct;
	SaveEntry* saveUpgradeStruct;
	FvaultUnlocksStruct* vaultUnlocksStruct;
	SaveEntry* saveVaultUnlocksStruct;

	std::unique_ptr<Image> background;

	std::unique_ptr<Image> thumbnailBackground;
	std::unique_ptr<Image> thumbnail;
	std::unique_ptr<text> name;

	std::string nameString;
	std::string descriptionString;
	std::string buffString;
	std::string debuffString;

	std::unique_ptr<text> upgradeText;
	std::unique_ptr<text> buttonPriceText;
	std::unique_ptr<Image> currencyImg;
	uint32_t* currencyId;
	int* upgradeNum;
	int upgradeMax = 1;
	int* unlocked;
	double* price;
	double priceFallback;

public:
	std::unique_ptr<Ubutton> buyButton;
protected:
	void setupLocs() override;
private:
	void setLocAndSize(vector loc, vector size) override;


	bool mouseOver();

	void openWorld();
	void spawnPet();
	void equipBait();

	std::function<void()> callback;
};