#pragma once

#include "math.h"
#include "saveData.h"
#include "Hoverable.h"

#include <string>
#include <vector>

class Shader;
class Image;

class autoFisherUI;
class AFmoreInfoUI;

class Ubutton;
class animation;
struct Fcollision;

class AautoFisher : public IHoverable {
public:
	AautoFisher(int id);
	~AautoFisher();

	void Update(float deltaTime);
	void draw(Shader* shaderProgram);

	void leftClick();
	void rightClick();
	void collectFish();
	void catchFish();
	FfishData calcFish();
	std::vector<std::vector<float>> calcFishProbability(std::vector<FfishData> fishData, bool isCurrencyAFactor = true);
	int calcCurrencyInList(FfishData fish, std::vector<FsaveFishData> heldFish);
	std::vector<double> getUpgradeCost();
	double price(int level);
	double calcCurrencyHeld(std::vector<FsaveFishData> fishList = std::vector<FsaveFishData>(0));
	// gets the autofisher to start fishing again once it is no longer full
	void startFishing();

	void calcMouseOver(bool &mouseOverPrev, bool mouseOver);

	void upgrade();

	float getCatchTime();

	double calcIdleProfits(double afkTime);
	std::vector<vector> calcAutoFishList(int fishNum);
	std::vector<float> calcIdleFishChance(std::vector<FfishData> fishList);

	double calcFPS();
	double calcMPS();

	// used to display the correct color of auto fisher
	int upgradeAnimIndex;

	bool bMouseOver = false;

	std::unique_ptr<animation> anim;
	std::unique_ptr<animation> outline;
	std::unique_ptr<animation> fishingLine;

	vector loc;

	// collision
	void setupCollision();
	std::vector<vector> collisionPoints = std::vector<vector>{ {2, -59 + 65 }, { 18, -51 + 65 }, { 32, -56 + 64 }, { 16, -64 + 64 } };

	void calcIfPlayerInfront();
	bool inFrontPlayer = false;

	// world
	int* level;
	int maxLevel = 100;
	int maxCurrency = 100;

	std::vector<FsaveFishData> heldFish = std::vector<FsaveFishData>();

	std::unique_ptr<autoFisherUI> UI;

	float fishingPower = 50;

	// multiplier stuff
	int multiplier = 1;

	int currencyId = 1;

	int autoFisherNum = 0;

	std::unique_ptr<AFmoreInfoUI> afMoreInfoUI;

	int id = -1;

	// sets the stats (fullness, and speed) based on auto fisher level after load
	void setStats();

	std::unique_ptr<Fcollision> col;

private:
	static inline std::shared_ptr<Image> autoFisherSpriteSheet;
	static inline std::shared_ptr<Image> fishingLineSpriteSheet;
	static inline std::shared_ptr<Image> outlineSpriteSheet;
};