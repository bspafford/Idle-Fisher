#pragma once

#include <string>

#include "saveData.h"
#include "collision.h"
#include "animation.h"
#include "Image.h"
#include "comboOvertimeWidget.h"
#include "Audio.h"

class saveData;
class collision;
class Timer;
class AfishSchool;
class Shader;

class Acharacter {
public:
	Acharacter();

	void draw(Shader* shaderProgram);
	void DrawWidgets(Shader* shaderProgram);
	void drawFishingLine(Shader* shaderProgram);
	void Update(float deltaTime);

	// get the world location at the players feet
	static vector getCharLoc();

	void leftClick();
	void fishing();
	void StartFishing();
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

	float minFishTime = 5;
	float maxFishTime = 10;

	float catchTimer = 0;

	DeferredPtr<Timer> bobberCatchTimer;
	DeferredPtr<Timer> bobberBobTimer;
	float bobTime = 1.5;

	void animFinished();
	std::vector<vector> fishLineTipLocs;
	void setFishingTipLoc(int frame);

	// stops player from moving if they start fishing
	bool fishingStop = false;

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
	DeferredPtr<Timer> premiumCatchTimer;
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

	// fishing line locs
	std::vector<vector> castAnimLocs = std::vector<vector>{ { 95, 36 }, { 95, 36 }, { 94, 36 }, { 94, 36 }, { 95, 35 }, { 80, 76 }, { 48, 78 }, { 21, 34 }, { 27, 22 }, { 30, 21 }, { 47, 77 }, { 105, 66 }, { 105, 66 }, { 100, 73 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 } };
	std::vector<vector> idleFishing = std::vector<vector>{ { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 67 }, { 98, 66 }, { 98, 66 }, { 98, 66 }, { 98, 66 }, { 98, 66 } };
	std::vector<vector> transition = std::vector<vector>{ { 107, 63 }, { 105, 54 }, { 91, 50 } };
	std::vector<vector> pullAnim = std::vector<vector>{ { 96, 44 }, { 90, 48 }, { 97, 42 }, { 89, 52 } };
	std::vector<vector> catchAnim = std::vector<vector>{ { 97, 44 }, { 97, 52 }, { 60, 96 }, { 29, 79 }, { 32, 80 }, { 32, 80 }, { 32, 80 }, { 29, 81 }, { 51, 83 }, { 88, 73 }, { 95, 36 }, { 31, 81 } };

	std::unique_ptr<UcomboOvertimeWidget> comboOvertimeWidget;

	std::unique_ptr<Audio> catchFishAudio;
};

Acharacter* GetCharacter();