#pragma once

#include "widget.h"

#include "scrollBox.h"
#include "upgradeBox.h"
#include "button.h"

class npc;
class text;
class verticalBox;

class UfishermanWidget : public widget {
public:
	UfishermanWidget(widget* parent, npc* NPCParent);
	~UfishermanWidget();

	void draw(Shader* shaderProgram) override;
	void setup();
	void setupLocs() override;
	void showUpgradePage(int index);

	void setNameDescription(std::string name, std::string description);

	void updateStats();
	void upgradePower();
	void upgradeSpeed();
	void upgradeCatchChance();

	std::unique_ptr<UscrollBox> baitHolderList;
	std::unique_ptr<text> name;

private:
	void setPage1() {
		showUpgradePage(0);
	}
	void setPage2() {
		showUpgradePage(1);
	}

	// checks if the player doesn't have enough money for the upgrades, and will make text red
	void CheckTextColor();

	// returns the index of what current fishing rod the player is on
	int calcFishingRodIndex();
	// returns the index of what current fishing line the player is on
	int calcFishingLineIndex();
	// returns the index of what current bobber the player is on
	int calcBobberIndex();

	int selectedPageIndex = 0;

	double powerCost = 0;
	double speedCost = 0;
	double chanceCost = 0;

	class npc* NPCParent;

	// fishing rod page
	std::unique_ptr<Image> fishingRodThumbnail;
	std::unique_ptr<Image> fishingLineThumbnail;
	std::unique_ptr<Image> bobberThumbnail;
	std::unique_ptr<text> powerText;
	std::unique_ptr<Ubutton> powerUpgradeButton;
	std::unique_ptr<text> speedText;
	std::unique_ptr<Ubutton> speedUpgradeButton;
	std::unique_ptr<text> catchChanceText;
	std::unique_ptr<Ubutton> catchChanceUpgradeButton;
	std::unique_ptr<text> powerButtonPrice;
	std::unique_ptr<text> speedButtonPrice;
	std::unique_ptr<text> catchChanceButtonPrice;

	std::unique_ptr<text> powerLevelText;
	std::unique_ptr<text> speedLevelText;
	std::unique_ptr<text> catchChanceLevelText;

	// fishing rod stats
	std::unique_ptr<text> powerStatsText;
	std::unique_ptr<text> powerStatsTextNum;
	std::unique_ptr<text> speedStatsText;
	std::unique_ptr<text> speedStatsTextNum;
	std::unique_ptr<text> catchChanceStatsText;
	std::unique_ptr<text> catchChanceStatsTextNum;

	// upgrades
	std::vector<std::unique_ptr<Ubutton>> buttonList;
	std::unique_ptr<Image> upgradeBackground;

	// stats
	std::unique_ptr<verticalBox> nameHolder;
	std::unique_ptr<text> buffText;
	std::unique_ptr<text> debuffText;
	std::unique_ptr<Image> infoBackground;

	// npc img
	std::unique_ptr<Image> npcImg;
	std::unique_ptr<Image> npcBackground;

	std::unique_ptr<Image> selectedIcon;

	std::unique_ptr<Ubutton> closeButton;

	std::vector<std::unique_ptr<UupgradeBox>> upgradeBoxList;
};
