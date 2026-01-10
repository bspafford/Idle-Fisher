#pragma once

#include "widget.h"

#include "scrollBox.h"
#include "upgradeBox.h"
#include "button.h"

class npc;
class text;
class verticalBox;
class UprogressBar;
struct FmechanicStruct;
struct FsaveMechanicStruct;

class UmechanicWidget : public widget {
public:
	UmechanicWidget(widget* parent, npc* NPCParent);
	~UmechanicWidget();

	void draw(Shader* shaderProgram) override;
	void setupLocs() override;
	void setup();
	void setNameDescription(std::string name, std::string description);
	void buyFishTransporter();
	void upgradeFishTransporter();
	void update();

	double calcUpgradeCost();

	// checks if the player doesn't have enough money for the upgrades, and will make text red
	void CheckTextColor();

	double buyCost = 0;
	double upgradeCost = 0;

	npc* NPCParent;

	FmechanicStruct* mechanicStruct;
	FsaveMechanicStruct* saveMechanicStruct;

	// upgrades
	std::unique_ptr<Image> upgradeBackground;

	// stats
	std::unique_ptr<verticalBox> nameHolder;
	std::unique_ptr<text> name;
	std::unique_ptr<text> description;
	std::unique_ptr<Image> infoBackground;

	// npc img
	std::unique_ptr<Image> npcImg;
	std::unique_ptr<Image> npcBackground;

	std::unique_ptr<Ubutton> closeButton;

	// fish transporter
	std::unique_ptr<Ubutton> buyFishTransporterButton;
	std::unique_ptr<text> buyFishTransporterText;
	std::unique_ptr<text> buyFishTransporterPriceText;

	// bought screen
	std::unique_ptr<text> fishTransporterName;
	std::unique_ptr<Image> fishTransporterImg;
	std::unique_ptr<text> level;
	std::unique_ptr<UprogressBar> levelProgress;
	std::unique_ptr<text> maxHoldText;
	std::unique_ptr<text> maxHoldValue;
	std::unique_ptr<text> speedText;
	std::unique_ptr<text> speedValue;
	std::unique_ptr<text> collectSpeedText;
	std::unique_ptr<text> collectSpeedValue;
	std::unique_ptr<Ubutton> buyButton;
	std::unique_ptr<Ubutton> multi1x;
	std::unique_ptr<Ubutton> multi10x;
	std::unique_ptr<Ubutton> multiMax;

	std::unique_ptr<text> upgradePriceText;
	std::unique_ptr<Image> currencyIcon;
};
