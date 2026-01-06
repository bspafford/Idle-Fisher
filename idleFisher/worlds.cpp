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
	waterImg = std::make_unique<Image>("images/worlds/titleScreen/depthMap.png", vector{ 0, 0 }, false);

	std::unordered_map<std::string, animDataStruct> fishermanDockAnimData;
	fishermanDockAnimData.insert({ "anim", animDataStruct({0, 0}, {48, 0}, true) });
	fishermanDock = std::make_unique<animation>("worlds/titleScreen/characterDockSpriteSheet.png", 403, 303, fishermanDockAnimData, false);
	fishermanDock->SetAnchor(ANCHOR_RIGHT, ANCHOR_BOTTOM);
	fishermanDock->SetPivot({ 1.f, 0.f });
	fishermanDock->setAnimation("anim");
	fishermanDock->start();
	title = std::make_unique<Image>("images/worlds/titleScreen/title.png", vector{ 0.f, 0.f }, false);
	title->SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	title->SetPivot({ 0.f, 1.f });

	std::unordered_map<std::string, animDataStruct> treesAnimData;
	treesAnimData.insert({ "anim", animDataStruct({0, 0}, {14, 0}, true) });
	trees = std::make_unique<animation>("worlds/titleScreen/treesSpriteSheet.png", 337, 96, treesAnimData, false);
	trees->SetAnchor(ANCHOR_RIGHT, ANCHOR_TOP);
	trees->SetPivot({ 1.f, 1.f });
	trees->setAnimation("anim");
	trees->start();

	std::string startButtonPath;
	vector startButtonSize;
	if (std::filesystem::exists(SaveData::GetSaveDataPath())) { // if save exists, continue
		startButtonPath = "widget/pauseMenu/continue.png";
		startButtonSize = { 69.f, 20.f };
	} else {
		startButtonPath = "widget/pauseMenu/newGame.png";
		startButtonSize = { 66.f, 20.f };
	}
	startButton = std::make_unique<Ubutton>(nullptr, startButtonPath, startButtonSize.x, startButtonSize.y, 1, vector{ 45.f, 184.f }, false, false);
	startButton->addCallback(this, &titleScreen::startGame);

	settingsButton = std::make_unique<Ubutton>(nullptr, "widget/pauseMenu/settings.png", 70, 20, 1, vector{45.f, 152.f}, false, false);
	settingsButton->addCallback(this, &titleScreen::showSettings);

	exitButton = std::make_unique<Ubutton>(nullptr, "widget/pauseMenu/exit.png", 37, 20, 1, vector{ 45.f, 120.f }, false, false);
	exitButton->addCallback(this, &titleScreen::exit);

	transitionBox = std::make_unique<URectangle>(nullptr, vector{ 0, 0 }, stuff::screenSize, false, glm::vec4(0.f));

	fadeTimer = CreateDeferred<Timer>();
}

titleScreen::~titleScreen() {

}

void titleScreen::start() {
	// setup water shader images
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/titleScreen/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/titleScreen/depthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/titleScreen/reflections.png", "reflectionTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "", "underwaterObjectTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "", "underwaterObjectDepthMap");
	Scene::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(54.f / 255.f, 107.f / 255.f, 138.f / 255.f));
	Scene::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(206.f / 255.f, 210.f / 255.f, 158.f / 255.f));
	Scene::twoDWaterShader->setFloat("causticSize", 6.f);
	Scene::twoDWaterShader->setVec2("waterImgSize", glm::vec2(waterImg->getSize().x, waterImg->getSize().y));

	alpha = 0.f;
}

void titleScreen::startGame() {
	fadeTimer->addUpdateCallback(this, &titleScreen::fadeToBlack);
	fadeTimer->addCallback(this, &titleScreen::openWorld);
	fadeTimer->start(0.3f);
}

void titleScreen::showSettings() {
	Main::settingsWidget->addToViewport(nullptr);
}

void titleScreen::fadeToBlack() {
	alpha = fadeTimer->getTime() / fadeTimer->getMaxTime();
	if (transitionBox)
		transitionBox->setColor(glm::vec4(18 / 255.f, 11.f / 255.f, 22.f / 255.f, alpha));
}

void titleScreen::openWorld() {
	transitionBox->setColor(glm::vec4(18 / 255.f, 11.f / 255.f, 22.f / 255.f, 1.f)); // makes sure it goes to alpha 100% instead of like 99%
	Scene::openLevel("world1", WORLD_SET_LOC_NONE);
}

void titleScreen::exit() {
	glfwSetWindowShouldClose(Main::GetWindow(), true);
}

void titleScreen::draw(Shader* shaderProgram) {
	if (waterImg)
		waterImg->draw(Scene::twoDWaterShader);

	if (fishermanDock)
		fishermanDock->draw(shaderProgram);
	if (trees)
		trees->draw(shaderProgram);
	if (title)
		title->draw(shaderProgram);
	if (startButton)
		startButton->draw(shaderProgram);
	if (settingsButton)
		settingsButton->draw(shaderProgram);
	if (exitButton)
		exitButton->draw(shaderProgram);

	if (widget::getCurrWidget())
		widget::getCurrWidget()->draw(shaderProgram);

	if (transitionBox)
		transitionBox->draw(shaderProgram);
}

vaultWorld::vaultWorld() {
	map = new Image("images/vaultMap.png", { 20, -210 }, true);
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
	Scene::openLevel(SaveData::saveData.prevWorld, WORLD_SET_LOC_VAULT);
}

void vaultWorld::draw(Shader* shaderProgram) {
	map->draw(shaderProgram);

	AvaultPlacedItems::draw(shaderProgram);
	decorator->draw(shaderProgram);

	GetCharacter()->draw(shaderProgram);

	houseDoor->draw(shaderProgram);
	decorator->calcIfPlayerInfront();

	Main::drawWidgets(shaderProgram);
}

rebirthWorld::rebirthWorld() {
	SaveData::saveData.playerLoc = { 0, 0 };
	fishGod = new AfishGod({ 570, 700 });

	rebirthWorldImg = new Image("images/worlds/rebirth/rebirthWorld.png", vector{ 0, 0 }, true);

	waterTimer = CreateDeferred<Timer>();
	waterTimer->addCallback(rebirthWorld::addAnim);
	waterTimer->start(.13f * 3.f);

	rebirthExit = new ArebirthExit({ 600, 375 });

	doorGlow = new Image("images/worlds/rebirth/doorGlow.png", { 750-99, 156 }, true);

	waterImg = new Image("images/worlds/rebirth/depthMap.png", { 0, 0 }, true);
}

void rebirthWorld::deconstructor() {
	delete fishGod;
	delete rebirthWorldImg;
	delete rebirthExit;
	delete doorGlow;
	delete waterImg;

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
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/rebirth/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/rebirth/depthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/rebirth/reflectionTexture.png", "reflectionTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/rebirth/underwaterObjectTexture.png", "underwaterObjectTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/rebirth/underwaterObjectDepthMap.png", "underwaterObjectDepthMap");
	Scene::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(0, 64.f/255.f, 81.f/255.f));
	Scene::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(0, 130.f/255.f, 121.f/255.f));
	Scene::twoDWaterShader->setFloat("causticSize", 16.f);
	Scene::twoDWaterShader->setVec2("waterImgSize", glm::vec2(waterImg->getSize().x, waterImg->getSize().y));
}

void rebirthWorld::draw(Shader* shaderProgram) {
	waterImg->draw(Scene::twoDWaterShader);

	shaderProgram->Activate();
	
	charMoveDir = GetCharacter()->moveDir;

	//rebirthWorldImg->draw(shaderProgram);

	rebirthExit->draw(shaderProgram);
	fishGod->draw(shaderProgram);

	for (int i = 0; i < waterRippleList.size(); i++)
		waterRippleList[i]->draw(shaderProgram);
	GetCharacter()->draw(shaderProgram);

	removeAnim();

	// if movement goes from 0 to !0 then stop timer and set it to .13f * 3.f
	if (math::length(charPrevMoveDir) == 0 && math::length(charMoveDir) != 0 && waterTimer->getMaxTime() != .13f * 3.f) {
		waterTimer->stop();
		waterTimer->start(.13f * 3.f);
	}

	charPrevMoveDir = GetCharacter()->moveDir;
}

void rebirthWorld::addAnim() {
	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "anim", animDataStruct({0, 0}, {6, 0}, false) });
	animation* waterRipples = new animation("walkRipples.png", 48, 26, animData, true, SaveData::saveData.playerLoc - vector{12, 8}); // Acharacter::getCharLoc()
	waterRipples->setAnimation("anim");
	waterRipples->start();
	waterRippleList.push_back(waterRipples);
	if (math::length(GetCharacter()->moveDir) != 0)
		waterTimer->start(.13f * 3.f);
	else
		waterTimer->start(.13f * 6.f);
}

void rebirthWorld::removeAnim() {
	for (int i = waterRippleList.size() - 1; i >= 0; i--) {
		if (waterRippleList[i]->IsFinished()) {
			waterRippleList[i]->stop();
			waterRippleList.erase(waterRippleList.begin() + i);
		}
	}
}

world::world() {
	rain = std::make_unique<Arain>();
	rainStartTimer = CreateDeferred<Timer>();
	rainStartTimer->addCallback(this, &world::startRain);
	rainDurationTimer = CreateDeferred<Timer>();
	rainDurationTimer->addCallback(this, &world::stopRain);
	darkenScreenTimer = CreateDeferred<Timer>();
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
	fishSchoolSpawnTimer = CreateDeferred<Timer>();
	fishSchoolSpawnTimer->addCallback(this, &world::spawnFishSchool);
	fishSchoolSpawnTimer->start(math::randRange(upgrades::calcMinFishSchoolSpawnInterval(), upgrades::calcMaxFishSchoolSpawnInterval()));

	// bind texture stuff for water
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/water.png", "underwaterTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/waterDepthMap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/reflectionTexture.png", "reflectionTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/underwaterObjectTexture.png", "underwaterObjectTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/underwaterObjectDepthMap.png", "underwaterObjectDepthMap");
	Scene::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(54.f/255.f, 107.f/255.f, 138.f/255.f));
	Scene::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(206.f / 255.f, 210.f / 255.f, 158.f / 255.f));
	Scene::twoDWaterShader->setFloat("causticSize", 16.f);
	Scene::twoDWaterShader->setVec2("waterImgSize", glm::vec2(waterImg->getSize().x, waterImg->getSize().y));
	setupAutoFishers();

	// load idle profits
	loadIdleProfits();
	if (autoFisherList.size() > 0 && SaveData::saveData.mechanicStruct[0].unlocked) { // if has atleast 1 autofisher and has fish transporter
		Main::idleProfitWidget->addToViewport(nullptr);
	}

	buyer = std::make_unique<buyAutoFisher>(vector{ 295, -170 });

	circleAnim->Start();

	if (fishTransporter)
		fishTransporter->startPathFinding();
}

void world::loadIdleProfits() {
	if (!currWorld)
		return;
	
	float timeDiff = static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - SaveData::lastPlayed).count());

	if (currWorld->fishTransporter)
		currWorld->fishTransporter->calcIdleProfits(timeDiff);
	//if (currWorld->atm)
	//	currWorld->atm->calcIdleProfits(timeDiff);
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
	
	if (Scene::pet)
		Scene::pet->draw(shaderProgram);
	
	sortDraw(shaderProgram);

	//if (rain) {
	//	rain->draw(shaderProgram);
	//	// makes screen darker
	//	SDL_SetRenderDrawColor(shaderProgram, 0, 0, 0, darkenValue);
	//	SDL_Rect rect = { 0, 0, stuff::screenSize.x, stuff::screenSize.y };
	//	SDL_RenderFillRect(shaderProgram, &rect);
	//}

	Main::drawWidgets(shaderProgram);

	if (widget::getCurrWidget())
		widget::getCurrWidget()->draw(shaderProgram);

	circleAnim->draw();
}

void world::renderWater() {
	if (waterImg)
		waterImg->draw(Scene::twoDWaterShader);

	glDisable(GL_DEPTH_TEST);

	// ==== DRAW 2D STUFF ====
	Scene::twoDShader->Activate();

	// draws ship inbetween water and dock
	if (ship)
		ship->draw(Scene::twoDShader);
	
	if (beachAnim)
		beachAnim->draw(Scene::twoDShader);
	if (mapImg)
		mapImg->draw(Scene::twoDShader);
}

void world::sortDraw(Shader* shaderProgram) {
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

	GetCharacter()->draw(shaderProgram);

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

void world::setWorldChangeLoc(WorldLoc worldChangeLoc) {
	if (worldChangeLoc == WORLD_SET_LOC_SAILOR)
		SaveData::saveData.playerLoc = spawnLoc;
	else if (worldChangeLoc == WORLD_SET_LOC_VAULT) {
		SaveData::saveData.playerLoc = houseLoc;
	} else if (worldChangeLoc == WORLD_SET_LOC_ZERO) {
		SaveData::saveData.playerLoc = { 0, 0 };
	}
}

// world 1
world1::world1(WorldLoc worldChangeLoc) {
	spawnLoc = { 517, 506 };
	houseLoc = { 1670, -870 };
	bankSellLoc = { 1000, 650 };

	setWorldChangeLoc(worldChangeLoc);

	mapImg = std::make_unique<Image>("images/map1.png", vector{ 0, 0 }, true);
	for (int i = 0; i < 19; i++)
		mapAnimList.push_back("images/worlds/world1/map" + std::to_string(i + 1) + ".png");
	ship = std::make_unique<Aship>(vector{ 336.f, 470.f });

	sellFish = std::make_unique<dumpster>(vector{ 849.f, 722.f });

	// npcs
	sailor = std::make_unique<Asailor>(vector{ 510, 544 });
	fisherman = std::make_unique<Afisherman>(vector{ 651.f, 731.f });
	atm = std::make_unique<Aatm>(vector{ 775.f, 736.f });
	scuba = std::make_unique<Ascuba>(vector{ 749.f, 412.f });
	petSeller = std::make_unique<ApetSeller>(vector{ 965.f, 800.f });
	merchant = std::make_unique<Amerchant>(vector{ 1053.f, 815.f });
	mechanic = std::make_unique<Amechanic>(vector{ 1120.f, 701.f });

	if (SaveData::saveData.mechanicStruct[0].unlocked)
		fishTransporter = std::make_unique<AfishTransporter>(vector{ 777.f, 711.f });

	// npc buildings
	house = std::make_unique<Ahouse>(vector{ 1157.f, 775.f });
	merchantShop = std::make_unique<AmerchantShop>(vector{ 998.f, 773.f });
	mechanicHouse = std::make_unique<AmechanicHouse>(vector{ 1130.f, 614.f });
	petShop = std::make_unique<ApetShop>(vector{ 930.f, 761.f });

	// make trees
	std::vector<vector> treeLocs = { 
		{ 1124.f, 1146.f }, { 1239.f, 1136.f }, { 1342.f, 1124.f }, { 1444.f, 1107.f }, { 729.f, 1102.f }, { 915.f, 1099.f },
		{ 1025.f, 1099.f }, { 1601.f, 1082.f }, { 1163.f, 1076.f }, { 810.f, 1070.f }, { 614.f, 1055.f }, { 975.f, 1054.f }, 
		{ 1273.f, 1048.f }, { 1074.f, 1047.f }, { 1512.f, 1047.f }, { 1369.f, 1044.f }, { 1209.f, 1031.f }, { 880.f, 1028.f }, 
		{ 715.f, 1024.f }, { 1435.f, 1007.f }, { 1122.f, 1006.f }, { 1655.f, 994.f }, { 1303.f, 993.f }, { 1563.f, 990.f },
		{ 1046.f, 989.f }, { 788.f, 971.f }, { 961.f, 960.f }, { 873.f, 954.f }, { 1369.f, 952.f }, { 1500.f, 945.f }, 
		{ 731.f, 920.f }, { 831.f, 901.f }, { 1422.f, 901.f }, { 1488.f, 869.f }, { 1410.f, 846.f } 
	};

	std::vector<vector> bushLocs = { 
		{ 1236.f, 965.f }, { 666.f, 940.f }, { 1292.f, 923.f }, { 1028.f, 911.f }, { 1109.f, 900.f },
		{ 908.f, 896.f }, { 766.f, 863.f }, { 1455.f, 770.f }
	};

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

	waterImg = std::make_unique<Image>("images/water/water.png", vector{ 0, 0 }, true);

	// pole list
	std::vector<vector> poleLocs = { { 580, 478 },{ 622, 499 },{ 664, 520 }, { 706, 541 }, { 748, 562 }, { 790, 583 } };
	for (int i = 0; i < poleLocs.size(); i++) {
		std::unique_ptr<Image> poleImg = std::make_unique<Image>("images/worlds/world1/dockPole.png", poleLocs[i], true);
		poleList.push_back(std::move(poleImg));
	}
}

void world::finishedBeachAnim() {
	if (beachAnim->GetCurrAnim() == "in")
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

world2::world2(WorldLoc worldChangeLoc) {

}

world3::world3(WorldLoc worldChangeLoc) {
	
}

// world 1
world4::world4(WorldLoc worldChangeLoc) {
	
}

world5::world5(WorldLoc worldChangeLoc) {
	
}

world6::world6(WorldLoc worldChangeLoc) {
	
}

world7::world7(WorldLoc worldChangeLoc) {
	
}

world8::world8(WorldLoc worldChangeLoc) {
	
}

world9::world9(WorldLoc worldChangeLoc) {
	
}

world10::world10(WorldLoc worldChangeLoc) {
	
}
