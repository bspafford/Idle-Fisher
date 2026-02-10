#pragma once

#include <string>

#include "saveData.h"
#include "collision.h"
#include "animation.h"
#include "Image.h"
#include "comboOvertimeWidget.h"
#include "Audio.h"
#include "NumberWidget.h"
#include "Rectangle.h"

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
	void Update(float deltaTime);

	void setCanMove(bool move);
	bool getCanMove();

	// get the world location at the players feet
	vector getCharLoc();
	float GetSpeed();
	Fcollision* GetCollision();
	vector GetMoveDir();
	bool GetIsFishing();
	bool IsFishingInSchool();

	double GetCombo();
	// Can be a negative value
	void IncreaseCombo(double comboChange);

	void equipBait(uint32_t baitId);

	bool IsBobberInRiver();

private:
	void drawFishingLine(Shader* shaderProgram);
	void leftClick();
	void fishing();
	void StartFishing();
	void stopFishing();

	void move(float deltaTime);

	void bobberCatchAnim();
	void bobberBobAnim();

	void animFinished();
	void setFishingTipLoc(int frame);
	// returns SE, SW, NW, NE depending on where the bobber is relative to the character
	std::string GetFishingDirection();

	void comboOvertimeFinished();

	void setPlayerColPoints();

	// returns false if the player doesn't have a strong enough fishing rod for the world
	// so if world 2 requires 60 power for first fish but you only have a 50 power fishing rod, then it will return false
	bool canCatchWorldFish();

	void premiumFishBuff();

	void setCatchPremium();

	friend Acharacter* GetCharacter(); // get a reference to the character

	void calcFishSchool();
	AfishSchool* bobberInFishSchool();
	AfishSchool* currFishSchool; // if cast in a fishSchool

	// fish stuff
	FfishData calcFish(int& quality, int& fishSize);
	std::vector<std::pair<uint32_t, double>> calcFishProbability(const std::unordered_map<uint32_t, FfishData>& fishData);

	static inline std::unique_ptr<animation> anim;
	std::unique_ptr<animation> fishingRod;

	// setup fish image
	std::unique_ptr<Image> fishImg;
	bool showFish = false;

	FfishData currFish;
	int currFishQuality;
	int currFishSize;

	static inline std::unique_ptr<Image> bobberImg;
	static inline std::unique_ptr<animation> bobberWaterAnimFront;
	static inline std::unique_ptr<animation> bobberWaterAnimBack;

	float speed = 75.f;

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

	std::vector<vector> fishLineTipLocs;
	// stops player from moving if they start fishing
	bool fishingStop = false;

	static inline std::unique_ptr<Fcollision> col;

	// fishing line stuff
	static inline vector fishingTipLoc = { 69, 60 };
	static inline bool showFishingLine = false;
	std::unique_ptr<URectangle> fishingLineRect;

	// if going can't catch golden fihs
	DeferredPtr<Timer> premiumCatchTimer;
	bool canCatchPremium = true;

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
	std::unique_ptr<Audio> catchPremiumAudio;
	std::unique_ptr<Audio> walkSFX;
	std::unique_ptr<Audio> castAudio;
	std::unique_ptr<Audio> tightRopeAudio;

	// Called when the animation frame that either foot has hit the floor
	void FootHitFloor();

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

	bool isBobberInRiver = false;
};

Acharacter* GetCharacter();