#pragma once

#include "math.h"
#include "saveData.h"
#include "Hoverable.h"

#include <string>
#include <vector>
#include <unordered_map>

class Shader;
class Image;

class autoFisherUI;
class AFmoreInfoUI;

class Ubutton;
class animation;
struct Fcollision;
class NumberWidget;
class Audio;

class AautoFisher : public IHoverable {
public:
	AautoFisher(uint32_t id);
	~AautoFisher();

	void Update(float deltaTime);
	void draw(Shader* shaderProgram);

	void leftClick();
	void rightClick();
	void collectFish();
	void catchFish();
	FfishData calcFish();
	std::vector<std::pair<uint32_t, float>> calcFishProbability(const std::unordered_map<uint32_t, FfishData>&, bool isCurrencyAFactor = true);
	// returns id, level, price
	std::tuple<uint32_t, int, double> getUpgradeCost();
	double price(int level);
	double calcCurrencyHeld();
	// gets the autofisher to start fishing again once it is no longer full
	void startFishing();

	void calcMouseOver(bool &mouseOverPrev, bool mouseOver);

	void upgrade();

	float getCatchTime();

	double calcIdleProfits(double afkTime);
	// fishId, fishNum
	std::vector<std::pair<uint32_t, double>> calcAutoFishList(int fishNum);
	std::unordered_map<uint32_t, float> calcIdleFishChance(std::unordered_map<uint32_t, FfishData> fishList);

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

	std::unordered_map<uint32_t, FsaveFishData> heldFish;

	std::unique_ptr<autoFisherUI> UI;

	float fishingPower = 50;

	// multiplier stuff
	int multiplier = 1;

	int autoFisherNum = 0;

	std::unique_ptr<AFmoreInfoUI> afMoreInfoUI;

	uint32_t id = 0;
	uint32_t worldId = 0;

	// sets the stats (fullness, and speed) based on auto fisher level after load
	void setStats();

	std::unique_ptr<Fcollision> col;

private:
	void OutlineUpdate(int frame);

	static inline std::shared_ptr<Image> autoFisherSpriteSheet;
	static inline std::shared_ptr<Image> fishingLineSpriteSheet;
	static inline std::shared_ptr<Image> outlineSpriteSheet;

	// recasting
	void StartRecast(uint32_t fishId, double caughtNum);
	void Recast();
	DeferredPtr<Timer> recastTimer;
	std::unique_ptr<Audio> recastAudio;
	double chainChance = 0.0; // keeps track of current chain chance
	bool recastActive = false;
	int recastNum = 0; // how many recasts
	uint32_t fishAtStartOfRecast;
	double catchNumAtStartOfRecast = 0;

	std::unique_ptr<NumberWidget> numberWidget;
};