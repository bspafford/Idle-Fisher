#include "worlds.h"
#include "main.h"
#include "character.h"
#include "AautoFisher.h"
#include "buyAutoFisher.h"
#include "dumpster.h"
#include "vaultPlacedItems.h"
#include "fishSchool.h"
#include "ship.h"
#include "upgrades.h"
#include "Image.h"
#include "Texture.h"
#include "Rectangle.h"
#include "CircleLoad.h"

#include "button.h"
#include "animation.h"
#include "timer.h"
#include "rebirthExit.h"

// npcs
#include "pet.h"
#include "sailor.h"
#include "fisherman.h"
#include "atm.h"
#include "scuba.h"
#include "petSeller.h"
#include "merchant.h"
#include "mechanic.h"
#include "decorator.h"
#include "fishTransporter.h"
#include "fishGod.h"

// npc buildings
#include "house.h"
#include "merchantShop.h"
#include "mechanicHouse.h"
#include "petShop.h"
#include "rock.h"
#include "rain.h"

#include "debugger.h"

// title screen
titleScreen::titleScreen() {
	waterImg = std::make_unique<Image>("./images/worlds/titleScreen/depthMap.png", vector{ 0, 0 }, false);

	std::unordered_map<std::string, animDataStruct> fishermanDockAnimData;
	fishermanDockAnimData.insert({ "anim", animDataStruct({0, 0}, {48, 0}, true) });
	fishermanDock = std::make_unique<animation>("worlds/titleScreen/characterDockSpriteSheet.png", 403, 303, fishermanDockAnimData, false);
	fishermanDock->setAnimation("anim");
	fishermanDock->start();
	title = std::make_unique<Image>("./images/worlds/titleScreen/title.png", vector{ 0, 0 }, false);

	std::unordered_map<std::string, animDataStruct> treesAnimData;
	treesAnimData.insert({ "anim", animDataStruct({0, 0}, {14, 0}, true) });
	trees = std::make_unique<animation>("worlds/titleScreen/treesSpriteSheet.png", 337, 96, treesAnimData, false);
	trees->setAnimation("anim");
	trees->start();

	newGameButton = std::make_unique<Ubutton>(nullptr, "widget/pauseMenu/newGame.png", 66, 20, 1, vector{ 45.f * stuff::pixelSize, 150.f * stuff::pixelSize }, false, false);
	newGameButton->addCallback(this, &titleScreen::newGame);

	continueButton = std::make_unique<Ubutton>(nullptr, "widget/pauseMenu/continue.png", 69, 20, 1, vector{ 45.f * stuff::pixelSize, 182.f * stuff::pixelSize }, false, false);
	continueButton->addCallback(this, &titleScreen::continueGame);

	exitButton = std::make_unique<Ubutton>(nullptr, "widget/pauseMenu/exit.png", 37, 20, 1, vector{ 45.f * stuff::pixelSize, 214.f * stuff::pixelSize }, false, false);
	exitButton->addCallback(this, &titleScreen::exit);

	transitionBox = std::make_unique<URectangle>(vector{ 0, 0 }, stuff::screenSize, false, glm::vec4(0.f));

	fadeTimer = std::make_unique<timer>();
}

titleScreen::~titleScreen() {

}

void titleScreen::start() {
	// setup water shader images
	Texture::bindTextureToShader({ Main::waterShader, Main::twoDWaterShader }, "./images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/titleScreen/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader({ Main::twoDWaterShader, Main::twoDShader }, "./images/worlds/titleScreen/depthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/titleScreen/reflections.png", "reflectionTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "", "underwaterObjectTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "", "underwaterObjectDepthMap");
	Main::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(54.f / 255.f, 107.f / 255.f, 138.f / 255.f));
	Main::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(206.f / 255.f, 210.f / 255.f, 158.f / 255.f));
	Main::twoDWaterShader->setFloat("causticSize", 6.f);
}

void titleScreen::newGame() {
	fadeTimer->addUpdateCallback(this, &titleScreen::fadeToBlack);
	fadeTimer->start(0.3f);
	fadeTimer->addCallback(this, &titleScreen::openWorld);
}

void titleScreen::continueGame() {
	fadeTimer->addUpdateCallback(this, &titleScreen::fadeToBlack);
	fadeTimer->addCallback(this, &titleScreen::openWorld);
	fadeTimer->start(0.3f);
	//openWorld();
}

void titleScreen::fadeToBlack() {
	alpha = fadeTimer->getTime() / fadeTimer->getMaxTime();
	if (transitionBox)
		transitionBox->setColor(glm::vec4(18 / 255.f, 11.f / 255.f, 22.f / 255.f, alpha));
}

void titleScreen::openWorld() {
	fadeToBlack(); // makes sure it goes to alpha 100% instead of like 99%
	Scene::openLevel("world1");
	alpha = 0;
}

void titleScreen::exit() {
	glfwSetWindowShouldClose(Main::window, true);
}

void titleScreen::draw(Shader* shaderProgram) {
	if (waterImg)
		waterImg->draw(Main::twoDWaterShader); // Main::twoDWaterShader

	// set bottom right
	if (fishermanDock)
		fishermanDock->setLoc(stuff::screenSize - fishermanDock->GetCellSize() * stuff::pixelSize);
	// set top left
	if (title)
		title->setLoc({0, 0});
	// set top right
	if (trees) {
		trees->setLoc({ stuff::screenSize.x - trees->GetCellSize().x * stuff::pixelSize, 0 });
		trees->draw(shaderProgram);
	}
	if (fishermanDock)
		fishermanDock->draw(shaderProgram);
	if (title)
		title->draw(shaderProgram);
	if (newGameButton)
		newGameButton->draw(shaderProgram);
	if (continueButton)
		continueButton->draw(shaderProgram);
	if (exitButton)
		exitButton->draw(shaderProgram);

	if (transitionBox)
	transitionBox->draw(shaderProgram);
}

vaultWorld::vaultWorld() {
	map = new Image("./images/vaultMap.png", { 20, -210 }, true);
	houseDoor = new Ubutton(NULL, "houseDoor.png", 23, 62, 1, { 115, -13 }, true, true);
	houseDoor->addCallback(vaultWorld::leaveHouse);

	decorator = new Adecorator({ 50, -10 });

	AvaultPlacedItems();
}

void vaultWorld::deconstructor() {
	delete houseDoor;
	delete decorator;
	map = nullptr;
	houseDoor = nullptr;
	decorator = nullptr;
}

void vaultWorld::start() {
	SaveData::saveData.playerLoc = { 546, 46 }; // 550, 55
	//circleAnim->start();
}

void vaultWorld::leaveHouse() {
	Scene::openLevel(SaveData::saveData.prevWorld, worldLoc::leaveVault);
}

void vaultWorld::draw(Shader* shaderProgram) {
	map->draw(shaderProgram);

	AvaultPlacedItems::draw(shaderProgram);
	decorator->draw(shaderProgram);

	Main::character->draw(shaderProgram);

	houseDoor->draw(shaderProgram);
	decorator->calcIfPlayerInfront();

	Main::drawWidgets(shaderProgram);
}

rebirthWorld::rebirthWorld() {
	SaveData::saveData.playerLoc = { 0, 0 };
	fishGod = new AfishGod({ 570, 700 });

	rebirthWorldImg = new Image("./images/worlds/rebirth/rebirthWorld.png", vector{ 0, 0 }, true);

	waterTimer = new timer();
	waterTimer->addCallback(rebirthWorld::addAnim);
	waterTimer->start(.13f * 3.f);

	rebirthExit = new ArebirthExit({ 600, 375 });

	doorGlow = new Image("./images/worlds/rebirth/doorGlow.png", { 750-99, 156 }, true);

	waterImg = new Image("./images/worlds/rebirth/depthMap.png", { 0, 0 }, true);
}

void rebirthWorld::deconstructor() {
	delete waterTimer;
	delete fishGod;
	delete rebirthWorldImg;
	delete rebirthExit;
	delete doorGlow;
	delete waterImg;

	waterTimer = nullptr;
	fishGod = nullptr;
	rebirthWorldImg = nullptr;
	rebirthExit = nullptr;
	doorGlow = nullptr;
	waterImg = nullptr;

	for (int i = 0; i < waterRippleList.size(); i++)
		delete waterRippleList[i];
	waterRippleList.clear();
}

void rebirthWorld::start() {
	// setup water shader images
	Texture::bindTextureToShader({ Main::waterShader, Main::twoDWaterShader }, "./images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/rebirth/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader({ Main::twoDWaterShader, Main::twoDShader }, "./images/worlds/rebirth/depthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/rebirth/reflectionTexture.png", "reflectionTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/rebirth/underwaterObjectTexture.png", "underwaterObjectTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/worlds/rebirth/underwaterObjectDepthMap.png", "underwaterObjectDepthMap");
	Main::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(0, 64.f/255.f, 81.f/255.f));
	Main::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(0, 130.f/255.f, 121.f/255.f));
	Main::twoDWaterShader->setFloat("causticSize", 16.f);
}

void rebirthWorld::draw(Shader* shaderProgram) {
	waterImg->draw(Main::twoDWaterShader);

	shaderProgram->Activate();
	
	charMoveDir = Main::character->moveDir;

	//rebirthWorldImg->draw(shaderProgram);

	rebirthExit->draw(shaderProgram);
	fishGod->draw(shaderProgram);

	for (int i = 0; i < waterRippleList.size(); i++)
		waterRippleList[i]->draw(shaderProgram);
	Main::character->draw(shaderProgram);

	removeAnim();

	// if movement goes from 0 to !0 then stop timer and set it to .13f * 3.f
	if (math::length(charPrevMoveDir) == 0 && math::length(charMoveDir) != 0 && waterTimer->getMaxTime() != .13f * 3.f) {
		waterTimer->stop();
		waterTimer->start(.13f * 3.f);
	}

	charPrevMoveDir = Main::character->moveDir;
}

void rebirthWorld::addAnim() {
	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "anim", animDataStruct({0, 0}, {6, 0}, false) });
	animation* waterRipples = new animation("walkRipples.png", 48, 26, animData, true, SaveData::saveData.playerLoc - vector{12, 8}); // Acharacter::getCharLoc()
	waterRipples->setAnimation("anim");
	waterRipples->start();
	waterRippleList.push_back(waterRipples);
	if (math::length(Main::character->moveDir) != 0)
		waterTimer->start(.13f * 3.f);
	else
		waterTimer->start(.13f * 6.f);
}

void rebirthWorld::removeAnim() {
	for (int i = waterRippleList.size() - 1; i >= 0; i--) {
		if (waterRippleList[i]->finished()) {
			waterRippleList[i]->stop();
			waterRippleList.erase(waterRippleList.begin() + i);
		}
	}
}

world::world() {
	rain = std::make_unique<Arain>();
	rainStartTimer = std::make_unique<timer>();
	rainStartTimer->addCallback(this, &world::startRain);
	rainDurationTimer = std::make_unique<timer>();
	rainDurationTimer->addCallback(this, &world::stopRain);
	darkenScreenTimer = std::make_unique<timer>();
	darkenScreenTimer->addUpdateCallback(this, &world::darkenScreen);
	darkenValue = 0;
	isRaining = false;
	circleAnim = std::make_unique<CircleLoad>();

	rainStartTimer->start(math::randRange(upgrades::calcMinRainSpawnInterval(), upgrades::calcMaxRainSpawnInterval()));
}

void world::startRain() {
	isRaining = true;
	darkenScreenTimer->start(1);
	rain->start();

	rainDurationTimer->start(math::randRange(minRainDurationTime, maxRainDurationTime));
}

void world::stopRain() {
	isRaining = false;
	darkenScreenTimer->start(1);
	rain->stop();

	rainStartTimer->start(math::randRange(upgrades::calcMinRainSpawnInterval(), upgrades::calcMaxRainSpawnInterval()));
}

void world::darkenScreen() {
	if (isRaining) // get darker
		darkenValue = darkenScreenTimer->getTime() / darkenScreenTimer->getMaxTime() * 100;
	else // brighten up
		darkenValue = (1 - darkenScreenTimer->getTime() / darkenScreenTimer->getMaxTime()) * 100;
}

world::~world() {

}

void world::start() {
	// on init make the circle appear
	fishSchoolSpawnTimer = std::make_unique<timer>();
	fishSchoolSpawnTimer->addCallback(this, &world::spawnFishSchool);
	fishSchoolSpawnTimer->start(math::randRange(upgrades::calcMinFishSchoolSpawnInterval(), upgrades::calcMaxFishSchoolSpawnInterval()));

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << err << std::endl;
	}
	// bind texture stuff for water
	Texture::bindTextureToShader({ Main::waterShader, Main::twoDWaterShader }, "./images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader({ Main::twoDWaterShader, Main::twoDShader }, "./images/water/waterDepthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/reflectionTexture.png", "reflectionTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/underwaterObjectTexture.png", "underwaterObjectTexture");
	Texture::bindTextureToShader(Main::twoDWaterShader, "./images/water/underwaterObjectDepthMap.png", "underwaterObjectDepthMap");
	Main::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(54.f/255.f, 107.f/255.f, 138.f/255.f));
	Main::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(206.f / 255.f, 210.f / 255.f, 158.f / 255.f));
	Main::twoDWaterShader->setFloat("causticSize", 16.f);
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << err << std::endl;
	}
	setupAutoFishers();

	// load idle profits
	Main::loadIdleProfits();
	if (autoFisherList.size() > 0 && SaveData::saveData.mechanicStruct[0].unlocked) { // if has atleast 1 autofisher and has fish transporter
		Main::idleProfitWidget->addToViewport(true);
	}

	buyer = std::make_unique<buyAutoFisher>(vector{ 295, -170 });

	circleAnim->Start();
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << "OpenGL Error: " << err << std::endl;
	}
}

void world::spawnFishSchool() {
	if (fishSchoolList.size() < maxFishSchoolNum) { // only spawn school if less than max
		float x = math::randRange(-200.f, 450.f);
		float y = math::randRange(-200.f, 150.f);
		std::unique_ptr<AfishSchool> fishSchool = std::make_unique<AfishSchool>(vector{ x, y });
		fishSchoolList.push_back(std::move(fishSchool));
	}
	fishSchoolSpawnTimer->start(math::randRange(upgrades::calcMinFishSchoolSpawnInterval(), upgrades::calcMaxFishSchoolSpawnInterval()));
}

void world::removeFishSchool(AfishSchool* fishSchool) {
	auto it = std::find_if(fishSchoolList.begin(), fishSchoolList.end(),
		[fishSchool](const std::unique_ptr<AfishSchool>& ptr) {
			return ptr.get() == fishSchool;
		});

	if (it != fishSchoolList.end())
		fishSchoolList.erase(it);
}

void world::spawnFishTransporter() {
	fishTransporter = std::make_unique<AfishTransporter>(vector{ 1000.f, 650.f });
	makeDrawLists();
}
void world::makeDrawLists() {
	npcList.clear();
	buildingList.clear();

	// npcs
	npcList.push_back(sailor.get());
	npcList.push_back(fisherman.get());
	npcList.push_back(atm.get());
	npcList.push_back(scuba.get());
	npcList.push_back(petSeller.get());
	npcList.push_back(merchant.get());
	npcList.push_back(mechanic.get());
	npcList.push_back(fishTransporter.get());

	// buildings
	if (house)
		buildingList.push_back(house.get());
	if (merchantShop)
		buildingList.push_back(merchantShop.get());
	if (mechanicHouse)
		buildingList.push_back(mechanicHouse.get());
	if (petShop)
		buildingList.push_back(petShop.get());

	std::vector<vector> rockLocs = { { 1068, 699 }, { 1379, 689 } };
	for (int i = 0; i < rockLocs.size(); i++) {
		std::unique_ptr<Arock> rock = std::make_unique<Arock>(rockLocs[i]);
		buildingList.push_back(rock.get());
		rockList.push_back(std::move(rock));
	}
}

void world::draw(Shader* shaderProgram) {
	renderWater();

	for (int i = 0; i < fishSchoolList.size(); i++)
		fishSchoolList[i]->draw(shaderProgram);

	for (int i = 0; i < trees.size(); i++)
		trees[i]->draw(shaderProgram);

	if (sellFish)
		sellFish->draw(shaderProgram);
	
	if (Main::pet)
		Main::pet->draw(shaderProgram);
	
	sortDraw(shaderProgram);

	//if (rain) {
	//	rain->draw(shaderProgram);
	//	// makes screen darker
	//	SDL_SetRenderDrawColor(shaderProgram, 0, 0, 0, darkenValue);
	//	SDL_Rect rect = { 0, 0, stuff::screenSize.x, stuff::screenSize.y };
	//	SDL_RenderFillRect(shaderProgram, &rect);
	//}

	Main::drawWidgets(shaderProgram);

	circleAnim->draw();
}

void world::renderWater() {
	if (waterImg)
		waterImg->draw(Main::twoDWaterShader);

	glDisable(GL_DEPTH_TEST);

	// ==== DRAW 2D STUFF ====
	Main::twoDShader->Activate();

	// draws ship inbetween water and dock
	if (ship)
		ship->draw(Main::twoDShader);
	
	if (beachAnim)
		beachAnim->draw(Main::twoDShader);
	if (mapImg)
		mapImg->draw(Main::twoDShader);
}

void world::sortDraw(Shader* shaderProgram) {
	// should have some sort of big list that sorts all the dynamic and static accordingly
	// shouldn't have it broken up into a bunch of different lists

	// sort all the draws to render some infront or behind depending on location
	
	// find the the lowest pixel in the far left side of the image, and do the same for the right?
	// then use those two location to test against the player

	//vector screenLoc = math::worldToScreen(loc);
	vector screenLoc = math::worldToScreen(SaveData::saveData.playerLoc, "topleft");

	std::vector<AautoFisher*> autoFishersBehind;
	std::vector<AautoFisher*> autoFishersInFront;
	std::vector<npc*> npcBehind;
	std::vector<npc*> npcInFront;
	std::vector<Abuilding*> buildingBehind;
	std::vector<Abuilding*> buildingInFront;
	std::vector<Image*> dockPoleInFront;
	std::vector<Image*> dockPoleBehind;

	// sort
	// calc autofisher
	for (int i = 0; i < autoFisherList.size(); i++) {
		if (autoFisherList[i]->inFrontPlayer)
			autoFishersInFront.push_back(autoFisherList[i].get());
		else
			autoFishersBehind.push_back(autoFisherList[i].get());
	}

	// calc npc
	for (npc* _npc : npcList) {
		if (!_npc)
			continue;
		if (_npc->calcIfPlayerInfront())
			npcBehind.push_back(_npc);
		else
			npcInFront.push_back(_npc);
	}

	for (Abuilding* building : buildingList) {
		if (building->calcInFront())
			buildingInFront.push_back(building);
		else
			buildingBehind.push_back(building);
	}

	vector characterPos = Acharacter::getCharLoc();
	for (std::unique_ptr<Image>& dockPole : poleList) {
		// if pole is further down on screen than character && if x is within range, so I don't have problem with autoFisher string overlap
		vector poleLoc = dockPole->getLoc();
		if (poleLoc.y < characterPos.y && characterPos.x >= poleLoc.x - 10.f && characterPos.x <= poleLoc.x + 10.f)
			dockPoleInFront.push_back(dockPole.get());
		else
			dockPoleBehind.push_back(dockPole.get());
	}


	for (int i = 0; i < buildingBehind.size(); i++)
		buildingBehind[i]->draw(shaderProgram);

	// draw npcs behind
	for (int i = 0; i < npcBehind.size(); i++) {
		npcBehind[i]->draw(shaderProgram);
	}

	for (Image* dockPole : dockPoleBehind) {
		dockPole->draw(shaderProgram);
	}

	// draw autofishers behind
	for (int i = 0; i < autoFishersBehind.size(); i++) {
		autoFishersBehind[i]->draw(shaderProgram);
	}

	if (buyer && !buyer->inFrontPlayer)
		buyer->draw(shaderProgram);

	Main::character->draw(shaderProgram);

	if (buyer && buyer->inFrontPlayer)
		buyer->draw(shaderProgram);

	for (int i = 0; i < buildingInFront.size(); i++)
		buildingInFront[i]->draw(shaderProgram);

	// draw npcs infront
	for (int i = 0; i < npcInFront.size(); i++) {
		npcInFront[i]->draw(shaderProgram);
	}

	for (Image* dockPole : dockPoleInFront) {
		dockPole->draw(shaderProgram);
	}

	// draw autofishers infront
	for (int i = 0; i < autoFishersInFront.size(); i++) {
		autoFishersInFront[i]->draw(shaderProgram);
	}
}

void world::setupAutoFishers() {
	autoFisherList.clear();
	// load autoFisher
	for (int i = 0; i < SaveData::saveData.autoFisherList.size(); i++) {
		if (SaveData::saveData.autoFisherList[i].unlocked && SaveData::data.autoFisherData[i].worldName == Scene::getCurrWorldName()) {
			std::unique_ptr<AautoFisher> autoFisher = std::make_unique<AautoFisher>(i);
			autoFisherList.push_back(std::move(autoFisher));
		}
	}
}

void world::setWorldChangeLoc(int worldChangeLoc) {
	if (worldChangeLoc == worldLoc::changeWorlds)
		SaveData::saveData.playerLoc = spawnLoc;
	else if (worldChangeLoc == worldLoc::leaveVault) {
		SaveData::saveData.playerLoc = houseLoc;
	} else if (worldChangeLoc == worldLoc::zero) {
		SaveData::saveData.playerLoc = { 0, 0 };
	}
}

// world 1
world1::world1(int worldChangeLoc) {
	spawnLoc = { 517, 476 };
	houseLoc = { 1670, -870 };
	bankSellLoc = { 1000, 650 };

	setWorldChangeLoc(worldChangeLoc);

	mapImg = std::make_unique<Image>("./images/map1.png", vector{ 0, 0 }, true);
	for (int i = 0; i < 19; i++)
		mapAnimList.push_back("./images/worlds/world1/map" + std::to_string(i + 1) + ".png");
	ship = std::make_unique<Aship>(vector{ -164 + 500, 495 - 25 });

	sellFish = std::make_unique<dumpster>(vector{ 739 + 110, 672 + 50 });

	// npcs
	sailor = std::make_unique<Asailor>(vector{ 10 + 500, -5 + 554 - 5 });
	fisherman = std::make_unique<Afisherman>(vector{ 143 + 8 + 500, 190 + 550 - 9 });
	atm = std::make_unique<Aatm>(vector{ 327 + 500 - 52, 186 + 547 - 9 + 12 });
	scuba = std::make_unique<Ascuba>(vector{ 354 - 105 + 500, -133 + 554 - 9 });
	petSeller = std::make_unique<ApetSeller>(vector{ 365 + 100 + 500, 255 + 554 - 9 });
	merchant = std::make_unique<Amerchant>(vector{ 143 + 410 + 500, 270 + 554 - 9 });
	mechanic = std::make_unique<Amechanic>(vector{ 560 + 60 + 500, 150 + 560 - 9 });

	if (SaveData::saveData.mechanicStruct[0].unlocked)
		fishTransporter = std::make_unique<AfishTransporter>(vector{ 417 - 140 + 500, 157 + 554 });

	// npc buildings
	house = std::make_unique<Ahouse>(vector{ 1157, 429 + 346 });
	merchantShop = std::make_unique<AmerchantShop>(vector{ 998, 368 + 405 });
	mechanicHouse = std::make_unique<AmechanicHouse>(vector{ 1130, 251 + 363 });
	petShop = std::make_unique<ApetShop>(vector{ 930, 321 + 440 });

	// make trees
	std::vector<vector> treeLocs = { {624, 524}, {739, 514},{842, 502},{944, 485},{229, 480},{415, 477},{525, 477},{1101, 460},{663, 454},{310, 448},{114, 433},{475, 432},{773, 426},{574, 425},{1012, 425},{869, 422},{709, 409},{380, 406},{215, 402},{935, 385},{622, 384},{1155, 372},{803, 371},{1063, 368},{546, 367},{288, 349},{461, 338},{373, 332},{869, 330},{1000, 323},{231, 298},{331, 279},{922, 279},{988, 247},{910, 224} };
	std::vector<vector> bushLocs = { { 736, 434 }, { 166, 409 }, { 792, 392 }, { 528, 380 }, { 609, 369 }, { 408, 365 }, { 266, 332 }, { 955, 239 } };

	// temp
	for (int i = 0; i < treeLocs.size(); i++)
		treeLocs[i] += vector{ 500, 622 };
	for (int i = 0; i < bushLocs.size(); i++)
		bushLocs[i] += vector{ 500, 531 };

	for (int i = 0; i < treeLocs.size(); i++)
		trees.push_back(std::make_unique<Atree>(treeLocs[i], true));

	for (int i = 0; i < bushLocs.size(); i++)
		trees.push_back(std::make_unique<Atree>(bushLocs[i], false));

	sortTreeList();
	makeDrawLists();

	std::unordered_map<std::string, animDataStruct> beachData;
	beachData.insert({ "in", animDataStruct({0, 0}, {14, 0}, false) });
	beachData.insert({ "out", animDataStruct({0, 1}, {14, 1}, false) });
	beachAnim = std::make_unique<animation>("water/beachSpriteSheet.png", 1152, 388, beachData, true, vector{ 708, 511 });
	beachAnim->setAnimation("in");
	beachAnim->addFinishedCallback(static_cast<world*>(this), &world::finishedBeachAnim);
	beachAnim->start();

	waterImg = std::make_unique<Image>("./images/water/water.png", vector{ 0, 0 }, true);

	// pole list
	std::vector<vector> poleLocs = { { 580, 478 },{ 622, 499 },{ 664, 520 }, { 706, 541 }, { 748, 562 }, { 790, 583 } };
	for (int i = 0; i < poleLocs.size(); i++) {
		std::unique_ptr<Image> poleImg = std::make_unique<Image>("./images/worlds/world1/dockPole.png", poleLocs[i], true);
		poleList.push_back(std::move(poleImg));
	}
}

void world::finishedBeachAnim() {
	if (beachAnim->currAnim == "in")
		beachAnim->setAnimation("out");
	else
		beachAnim->setAnimation("in");
	beachAnim->start();
}

void world::sortTreeList() {
	std::sort(trees.begin(), trees.end(),
		[](const std::unique_ptr<Atree>& a, const std::unique_ptr<Atree>& b) {
			return b->loc.y < a->loc.y;
		});
}

world2::world2(int worldChangeLoc) {

}

world3::world3(int worldChangeLoc) {
	
}

// world 1
world4::world4(int worldChangeLoc) {
	
}

world5::world5(int worldChangeLoc) {
	
}

world6::world6(int worldChangeLoc) {
	
}

world7::world7(int worldChangeLoc) {
	
}

world8::world8(int worldChangeLoc) {
	
}

world9::world9(int worldChangeLoc) {
	
}

world10::world10(int worldChangeLoc) {
	
}
