#pragma once

#include "stuff.h"
#include "math.h"
#include "Scene.h"

#include <string>
#include <stdio.h>
#include <vector>

#include "Image.h"

#include "tree.h"
#include "fishTransporter.h"
#include "AautoFisher.h"

#include "rain.h"
#include "FishBin.h"
#include "buyAutoFisher.h"
#include "CircleLoad.h"
#include "petShop.h"
#include "mechanicHouse.h"
#include "merchantShop.h"
#include "house.h"
#include "mechanic.h"
#include "merchant.h"
#include "petSeller.h"
#include "scuba.h"
#include "atm.h"
#include "fisherman.h"
#include "ship.h"
#include "sailor.h"
#include "rock.h"
#include "fishSchool.h"

class animation;
class Timer;
class Ubutton;
class Shader;
class URectangle;
class text;

class ArebirthExit;

class npc;
class Adecorator;
class AfishGod;

// npc buidings
class Abuilding;

class titleScreen {
public:
	titleScreen();
	~titleScreen();

	static inline std::unique_ptr<titleScreen> currTitleScreen;

	void start();
	void draw(Shader* shaderProgram);

	// water stuff
	std::unique_ptr<Image> waterImg;

	std::unique_ptr<animation> fishermanDock;
	std::unique_ptr<Image> title;
	std::unique_ptr<animation> trees;

	std::unique_ptr<Ubutton> startButton;
	std::unique_ptr<Ubutton> settingsButton;
	std::unique_ptr<Ubutton> exitButton;

	std::unique_ptr<URectangle> transitionBox;

	// button callback
	void startGame();
	void showSettings();
	void exit();

	void fadeToBlack();
	void openWorld();

	DeferredPtr<Timer> fadeTimer;
	float alpha;
};

class vaultWorld {
public:
	vaultWorld();
	static void deconstructor();
	static void start();
	static void draw(Shader* shaderProgram);
	
private:
	static void leaveHouse();

	static inline Image* map;

	static inline Ubutton* houseDoor;

	// npc
	static inline Adecorator* decorator;
};

class rebirthWorld {
public:
	rebirthWorld();
	static void deconstructor();
	static void start();
	static void draw(Shader* shaderProgram);

	static inline void addAnim();
	static inline void removeAnim();
	static inline std::vector<animation*> waterRippleList;
	static inline DeferredPtr<Timer> waterTimer;

	static inline AfishGod* fishGod;
	static inline Image* rebirthWorldImg;
	static inline ArebirthExit* rebirthExit;

	static inline vector charMoveDir;
	static inline vector charPrevMoveDir;

	static inline Image* doorGlow;

	// water stuff
	static inline Image* waterImg;

private:
	static inline float backgroundTime = 0;
};

class world {
public:
	world();
	
	static inline std::unique_ptr<world> currWorld;

	void start();
	void draw(Shader* shaderProgram);
	void setupAutoFishers();

	void spawnFishTransporter();
	static AfishTransporter* GetFishTransporter();
	static std::vector<std::unique_ptr<AautoFisher>>& GetAutoFisherList();

	void loadIdleProfits();

	std::unique_ptr<Image> mapImg;
	std::unique_ptr<animation> mapAnim;
	std::vector<std::string> mapAnimList;
	std::unique_ptr<Aship> ship;

	std::vector<std::unique_ptr<AfishSchool>> fishSchoolList;

	void makeDrawLists();
	// npcs
	std::vector<npc*> npcList; // list of all npcs in the world
	std::unique_ptr<Asailor> sailor;
	std::unique_ptr<Afisherman> fisherman;
	std::unique_ptr<Aatm> atm;
	std::unique_ptr<Ascuba> scuba;
	std::unique_ptr<ApetSeller> petSeller;
	std::unique_ptr<Amerchant> merchant;
	std::unique_ptr<Amechanic> mechanic;
	static inline std::unique_ptr<AfishTransporter> fishTransporter;

	// npc buildings
	std::vector<Abuilding*> buildingList; // list of all npcs in the world
	std::unique_ptr<Ahouse> house;
	std::unique_ptr<AmerchantShop> merchantShop;
	std::unique_ptr<AmechanicHouse> mechanicHouse;
	std::unique_ptr<ApetShop> petShop;
	std::vector<std::unique_ptr<Arock>> rockList;

	// trees
	std::vector<std::unique_ptr<Atree>> trees;
	void sortTreeList();

	void sortDraw(Shader* shaderProgram);

	std::unique_ptr<CircleLoad> circleAnim;

	static inline std::vector<std::unique_ptr<AautoFisher>> autoFisherList;
	std::unique_ptr<buyAutoFisher> buyer;
	std::unique_ptr<FishBin> fishBin;

	vector spawnLoc;
	vector houseLoc;

	void spawnFishSchool();
	void removeFishSchool(AfishSchool* fishSchool);
	DeferredPtr<Timer> fishSchoolSpawnTimer;
	int maxFishSchoolNum = 5;

	// rain
	void startRain();
	void stopRain();
	void darkenScreen();
	std::unique_ptr<Arain> rain;
	DeferredPtr<Timer> rainStartTimer;
	DeferredPtr<Timer> rainDurationTimer;
	DeferredPtr<Timer> darkenScreenTimer;
	float darkenValue = 0;
	bool isRaining = false;
	float minRainStartTime = 30*60, maxRainStartTime = 60*60;
	float minRainDurationTime = 5*60, maxRainDurationTime = 10*60;

	// water stuff
	void renderWater();
	void finishedBeachAnim();
	std::unique_ptr<Image> waterImg;
	std::unique_ptr<animation> beachAnim;

	// pole list
	std::vector<std::unique_ptr<Image>> poleList;

	// an image that will always be infront of the player no matter what
	std::unique_ptr<Image> inFront;

	vector bankSellLoc;

protected:
	void setWorldChangeLoc(WorldLoc worldChangeLoc);
};

class world1 : public world {
public:
	world1(WorldLoc worldChangeLoc);
};

class world2 : public world {
public:
	world2(WorldLoc worldChangeLoc);
};

class world3 : public world {
public:
	world3(WorldLoc worldChangeLoc);
};

class world4 : public world {
public:
	world4(WorldLoc worldChangeLoc);
};

class world5 : public world {
public:
	world5(WorldLoc worldChangeLoc);
};

class world6 : public world {
public:
	world6(WorldLoc worldChangeLoc);
};

class world7 : public world {
public:
	world7(WorldLoc worldChangeLoc);
};

class world8 : public world {
public:
	world8(WorldLoc worldChangeLoc);
};

class world9 : public world {
public:
	world9(WorldLoc worldChangeLoc);
};

class world10 : public world {
public:
	world10(WorldLoc worldChangeLoc);
};