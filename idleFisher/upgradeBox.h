#pragma once

#include "widget.h"

#include <functional>

class Image;
class text;
class Ubutton;
class verticalBox;
struct FbaitStruct;
struct FvaultUnlocksStruct;
struct SaveEntry;
struct ProgressionNode;
struct ModifierNode;
enum class Stat;

enum class UpgradeBoxType {
	Pet,
	Bait,
	Upgrade,
};

class UupgradeBox : public widget {
public:
	UupgradeBox(widget* parent, widget* NPCWidget, SaveEntry* saveWorldStruct);
	UupgradeBox(widget* parent, widget* NPCWidget, ModifierNode* data, SaveEntry* saveData, UpgradeBoxType type);
	UupgradeBox(widget* parent, widget* NPCWidget, FvaultUnlocksStruct* data, SaveEntry* saveData);
	~UupgradeBox();
	void setup(uint32_t progressId);
	void update();

	void draw(Shader* shaderProgram) override;

	vector getSize() override;

private:
	void buyUpgrade();

	widget* NPCWidget;

	// structs
	SaveEntry* saveWorldStruct;
	ModifierNode* baitStruct;
	SaveEntry* saveBaitStruct;
	ModifierNode* petStruct;
	SaveEntry* savePetStruct;
	ModifierNode* upgradeStruct;
	SaveEntry* saveUpgradeStruct;
	FvaultUnlocksStruct* vaultUnlocksStruct;
	SaveEntry* saveVaultUnlocksStruct;

	std::unique_ptr<Image> background;

	std::unique_ptr<Image> thumbnailBackground;
	std::unique_ptr<Image> thumbnail;
	std::unique_ptr<text> name;

	std::string nameString;
	std::string descriptionString;

	std::unique_ptr<text> upgradeText;
	std::unique_ptr<text> buttonPriceText;
	std::unique_ptr<Image> currencyImg;

	ProgressionNode* progressNode;
	SaveEntry* saveProgressNode;

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