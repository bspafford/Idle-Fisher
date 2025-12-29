#pragma once

#include <string>

#include "saveData.h"
#include "collision.h"
#include "animation.h"
#include "Image.h"

class saveData;
class collision;
class timer;
class AfishSchool;
class Shader;

class Acharacter {
public:
	Acharacter();

	void draw(Shader* shaderProgram);
	void drawFishingLine(Shader* shaderProgram);
	void Update(float deltaTime);

	// get the world location at the players feet
	static vector getCharLoc();

	void leftClick();
	void fishing();
	void stopFishing();

	void setCanMove(bool move);
	bool getCanMove();

	void bobberCatchAnim();
	void bobberBobAnim();

	void calcFishSchoolUpgrades();
	AfishSchool* bobberInFishSchool();
	static inline float fishSchoolMultiplier = 1.f; // increases the fish caught
	static inline float fishTimeMultiplier = 1.f; // reduces the catch time
	AfishSchool* currFishSchool; // if cast in a fishSchool

	vector getCharScreenLoc();

	// fish stuff
	FfishData calcFish(int& quality, int& fishSize);
	std::vector<std::pair<int, double>> calcFishProbability(std::vector<FfishData> fishData);

	static inline std::unique_ptr<animation> anim;
	std::unique_ptr<animation> fishingRod;

	// setup fish image
	std::unique_ptr<Image> fishImg;
	bool showFish = false;

	FfishData currFish;
	int currFishQuality;
	int currFishSize;

	static inline std::unique_ptr<Image> bobberImg;
	static inline std::unique_ptr<Image> bobberWaterOverlay;
	static inline std::unique_ptr<animation> bobberWaterAnimFront;
	static inline std::unique_ptr<animation> bobberWaterAnimBack;

	float speed = 85.f;

	void move(float deltaTime);

	vector moveDir;

	inline static bool isFishing = false;
	inline static vector bobberLoc;
	vector tempBobberLoc;

	// temp
	float minFishTime = 5;
	float maxFishTime = 10;

	float catchTimer = 0;

	std::unique_ptr<timer> fishingTimer;
	std::unique_ptr<timer> bobberCatchTimer;
	std::unique_ptr<timer> bobberBobTimer;
	float bobTime = 1.5;

	void animFinished();
	// temp
	std::vector<vector> fishLineTipLocs;
	void setFishingTipLoc(int frame);

	// stops player from moving if they start fishing
	bool fishingStop = false;

	std::unique_ptr<timer> comboOvertimer;
	void comboOvertimeFinished();

	static inline std::unique_ptr<Fcollision> col;
	static void setPlayerColPoints();

	// fishing line stuff
	static inline vector fishingTipLoc = { 69, 60 };
	static inline bool showFishingLine = false;

	// returns false if the player doesn't have a strong enough fishing rod for the world
	// so if world 2 requires 60 power for first fish but you only have a 50 power fishing rod, then it will return false
	bool canCatchWorldFish();

	void premiumFishBuff();

	// if going can't catch golden fihs
	std::unique_ptr<timer> premiumCatchTimer;
	bool canCatchPremium = true;
	void setCatchPremium();

	void equipFishingRod(FfishingRodStruct* fishingRod);
	void equipBait(FbaitStruct* bait);


	double GetCombo();
	// Can be a negative value
	void IncreaseCombo(double comboChange);

	friend Acharacter* GetCharacter(); // get a reference to the character

private:
	static inline Acharacter* characterPtr;

	bool canMove = true;
	vector prevMove = { 1, -1 };
	double comboNum = 1;
};

Acharacter* GetCharacter();