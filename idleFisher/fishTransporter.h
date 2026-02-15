#pragma once

#include "npc.h"
#include "math.h"
#include "AStar.h"
#include "path.h"
#include "Audio.h"

class AautoFisher;
class Timer;
class UprogressBar;
class animation;
class text;

class AfishTransporter : public npc {
public:
	AfishTransporter(vector loc);
	~AfishTransporter();

	void draw(Shader* shaderProgram) override;
	void update(float deltaTime);
	void startPathFinding();
	void SetStats();

	uint32_t GetId();

	float calcCollectTimer(double held, double afHeld, bool collectingFromAF);
	vector calcGoTo(int autoFisherIndex);
	
	void setLoc(vector loc) override;

	// fills held fish with random fish
	// called from calc idle profits
	// fillHeldFish will update the fish transporters held fish list, if false it wont touch it
	std::unordered_map<uint32_t, FsaveFishData> FillWithRandomFish(double currency, bool fillHeldFish);

private:
	void setupCollision() override;
	void setAnimation();
	// if getMaxTime is true it will return the maximum amount of time it will take for the fish transporter to collect its max fish
	// can pass in held to use that value instead of calcCurrencyHeld(), used for things like idle profit
	float calcCollectTimer(AautoFisher* autoFisher, bool getMaxTime = false);
	void finishCollectTimer();
	void collectTimerUpdate();
	void collectFish(AautoFisher* autoFisher);
	double CalcCapacity();
	// takes in auto fisher map and returns a sorted list of IDs
	// sorts list from biggest to smallest
	std::vector<uint32_t> sortFishList(const std::unordered_map<uint32_t, FsaveFishData>& map);
	void addFishtoHeld(FsaveFishData* fish, double addNum);
	void click() override;

	bool calcIfPlayerInfront() override;

	// Called when the animation frame that either foot has hit the floor
	void FootHitFloor();

	std::unique_ptr<AStar> Astar;

	std::unique_ptr<UprogressBar> progressBar;

	int autoFisherIndex;

	std::unique_ptr<animation> fishPileAnim;
	bool canMove = true;
	vector moveDir;
	vector prevMove = { 1, -1 };
	DeferredPtr<Timer> collectTimer;
	std::unique_ptr<text> fullnessText;

	std::unique_ptr<Audio> walkSFX;

	std::unordered_map<uint32_t, FsaveFishData> holding;

	std::string fullnessString = "empty";

	uint32_t id;
};