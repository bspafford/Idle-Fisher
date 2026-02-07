#include "worlds.h"
#include "main.h"
#include "character.h"
#include "AautoFisher.h"
#include "buyAutoFisher.h"
#include "FishBin.h"
#include "vaultPlacedItems.h"
#include "fishSchool.h"
#include "ship.h"
#include "upgrades.h"
#include "Image.h"
#include "Texture.h"
#include "Rectangle.h"
#include "CircleLoad.h"
#include "IdleProfits.h"

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
	Scene::openLevel(53u, WORLD_SET_LOC_NONE);
}

void titleScreen::exit() {
	glfwSetWindowShouldClose(Main::GetWindow(), true);
}

void titleScreen::draw(Shader* shaderProgram) {
	glDisable(GL_DEPTH_TEST);

	if (waterImg)
		waterImg->draw(Scene::twoDWaterShader);

	if (trees)
		trees->draw(shaderProgram);
	if (fishermanDock)
		fishermanDock->draw(shaderProgram);
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
	
	charMoveDir = GetCharacter()->GetMoveDir();

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

	charPrevMoveDir = GetCharacter()->GetMoveDir();
}

void rebirthWorld::addAnim() {
	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "anim", animDataStruct({0, 0}, {6, 0}, false) });
	animation* waterRipples = new animation("walkRipples.png", 48, 26, animData, true, SaveData::saveData.playerLoc - vector{12, 8}); // Acharacter::getCharLoc()
	waterRipples->setAnimation("anim");
	waterRipples->start();
	waterRippleList.push_back(waterRipples);
	if (math::length(GetCharacter()->GetMoveDir()) != 0)
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

	rainStartTimer->start(math::randRange(Upgrades::Get(Stat::MinRainSpawnInterval), Upgrades::Get(Stat::MaxRainSpawnInterval)));
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

	rainStartTimer->start(math::randRange(Upgrades::Get(Stat::MinRainSpawnInterval), Upgrades::Get(Stat::MaxRainSpawnInterval)));
}

void world::darkenScreen() {
	if (isRaining) // get darker
		darkenValue = darkenScreenTimer->getTime() / darkenScreenTimer->getMaxTime() * 100;
	else // brighten up
		darkenValue = (1 - darkenScreenTimer->getTime() / darkenScreenTimer->getMaxTime()) * 100;
}

void world::start() {
	// on init make the circle appear
	fishSchoolSpawnTimer = CreateDeferred<Timer>();
	fishSchoolSpawnTimer->addCallback(this, &world::spawnFishSchool);
	fishSchoolSpawnTimer->start(math::randRange(Upgrades::Get(Stat::MinFishSchoolSpawnInterval), Upgrades::Get(Stat::MaxFishSchoolSpawnInterval)));

	// bind texture stuff for water
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/waterDUDV.png", "dudvMap");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/demo/seaFloor.png", "underwaterTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/water/causticTexture.png", "causticTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/demo/depthmap.png", "waterDepthTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/demo/reflections.png", "reflectionTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/demo/underwater.png", "underwaterObjectTexture");
	Texture::bindTextureToShader(Scene::twoDWaterShader, "images/worlds/demo/objectDepthMap.png", "underwaterObjectDepthMap");
	Scene::twoDWaterShader->setVec3("deepWaterColor", glm::vec3(54.f/255.f, 107.f/255.f, 138.f/255.f));
	Scene::twoDWaterShader->setVec3("shallowWaterColor", glm::vec3(206.f / 255.f, 210.f / 255.f, 158.f / 255.f));
	Scene::twoDWaterShader->setFloat("causticSize", 16.f);
	Scene::twoDWaterShader->setVec2("waterImgSize", glm::vec2(waterImg->getSize().x, waterImg->getSize().y));
	setupAutoFishers();

	// load idle profits
	loadIdleProfits();
	if (autoFisherList.size() > 0 && SaveData::saveData.progressionData.at(90u).level) { // if has atleast 1 autofisher and has fish transporter
		Main::idleProfitWidget->addToViewport(nullptr);
	}

	buyer = std::make_unique<buyAutoFisher>(vector{ 295, -170 });

	grass->LoadGPU();

	circleAnim->Start();

	if (fishTransporter)
		fishTransporter->startPathFinding();
}

void world::loadIdleProfits() {
	if (!currWorld)
		return;
	
	float timeDiff = static_cast<float>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - SaveData::lastPlayed).count());

	IdleProfits::calcIdleProfits(timeDiff);
}

void world::spawnFishSchool() {
	if (fishSchoolList.size() < maxFishSchoolNum) { // only spawn school if less than max
		float x = math::randRange(-200.f, 450.f);
		float y = math::randRange(-200.f, 150.f);
		std::unique_ptr<AfishSchool> fishSchool = std::make_unique<AfishSchool>(vector{ x, y });
		fishSchoolList.push_back(std::move(fishSchool));
	}
	fishSchoolSpawnTimer->start(math::randRange(Upgrades::Get(Stat::MinFishSchoolSpawnInterval), Upgrades::Get(Stat::MaxFishSchoolSpawnInterval)));
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
	fishTransporter = std::make_unique<AfishTransporter>(vector(1320.f, 630.f));
	fishTransporter->startPathFinding();
	makeDrawLists();
}

AfishTransporter* world::GetFishTransporter() {
	return fishTransporter.get();
}

std::vector<std::unique_ptr<AautoFisher>>& world::GetAutoFisherList() {
	return autoFisherList;
}

void world::makeDrawLists() {
	npcList.clear();

	// npcs
	//npcList.push_back(sailor.get());
	//npcList.push_back(atm.get());
	//npcList.push_back(scuba.get());
	npcList.push_back(fisherman.get());
	npcList.push_back(petSeller.get());
	npcList.push_back(merchant.get());
	npcList.push_back(mechanic.get());
	npcList.push_back(fishTransporter.get());
}

void world::draw(Shader* shaderProgram) {
	renderWater();

	for (int i = 0; i < fishSchoolList.size(); i++)
		fishSchoolList[i]->draw(shaderProgram);
	
	if (Scene::pet)
		Scene::pet->draw(shaderProgram);
	
	sortDraw(shaderProgram);

	inFront->draw(shaderProgram);

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

	//for (auto& building : buildingList)
		//building->DrawDebugLines();
}

void world::renderWater() {
	if (waterImg)
		waterImg->draw(Scene::twoDWaterShader);

	Scene::twoDShader->Activate();

	// draws ship inbetween water and dock
	//if (ship)
		//ship->draw(Scene::twoDShader);
	
	//if (beachAnim)
		//beachAnim->draw(Scene::twoDShader);

	if (mapImg)
		mapImg->draw(Scene::twoDShader);

	if (grass)
		grass->Draw();
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

	for (auto& tree : trees)
		if (tree->calcIfPlayerInfront()) tree->draw(shaderProgram);

	// calc npc
	for (npc* _npc : npcList) {
		if (!_npc)
			continue;
		if (_npc->calcIfPlayerInfront())
			npcBehind.push_back(_npc);
		else
			npcInFront.push_back(_npc);
	}

	for (auto& building : buildingList) {
		if (building->calcInFront())
			buildingInFront.push_back(building.get());
		else
			buildingBehind.push_back(building.get());
	}

	vector characterPos = GetCharacter()->getCharLoc();
	for (std::unique_ptr<Image>& dockPole : poleList) {
		vector poleLoc = dockPole->getLoc();
		if (poleLoc.y < characterPos.y)
			dockPoleInFront.push_back(dockPole.get());
		else
			dockPoleBehind.push_back(dockPole.get());
	}

	bool fishBinInBehind = fishBin->calcIfPlayerInfront();
	if (fishBin && fishBinInBehind)
		fishBin->draw(shaderProgram);

	for (int i = 0; i < buildingBehind.size(); i++)
		buildingBehind[i]->draw(shaderProgram);

	// draw npcs behind
	for (int i = 0; i < npcBehind.size(); i++)
		npcBehind[i]->draw(shaderProgram);

	for (Image* dockPole : dockPoleBehind)
		dockPole->draw(shaderProgram);

	// draw autofishers behind
	for (int i = 0; i < autoFishersBehind.size(); i++)
		autoFishersBehind[i]->draw(shaderProgram);

	if (buyer && !buyer->inFrontPlayer)
		buyer->draw(shaderProgram);

	GetCharacter()->draw(shaderProgram);

	for (auto& tree : trees)
		if (!tree->calcIfPlayerInfront()) tree->draw(shaderProgram);

	for (int i = 0; i < buildingInFront.size(); i++)
		buildingInFront[i]->draw(shaderProgram);

	if (fishBin && !fishBinInBehind)
		fishBin->draw(shaderProgram);

	// draw npcs infront
	for (int i = 0; i < npcInFront.size(); i++)
		npcInFront[i]->draw(shaderProgram);

	if (buyer && buyer->inFrontPlayer)
		buyer->draw(shaderProgram);

	for (Image* dockPole : dockPoleInFront)
		dockPole->draw(shaderProgram);

	// draw autofishers infront
	for (int i = 0; i < autoFishersInFront.size(); i++)
		autoFishersInFront[i]->draw(shaderProgram);
}

void world::setupAutoFishers() {
	autoFisherList.clear();
	// load autoFisher
	for (uint32_t afId : SaveData::orderedData.autoFisherData) {
		ProgressionNode& afData = SaveData::data.progressionData.at(afId);
		SaveEntry& afSaveData = SaveData::saveData.progressionData.at(afId);
		if (afSaveData.level && afData.worldId == Scene::GetCurrWorldId()) {
			std::unique_ptr<AautoFisher> autoFisher = std::make_unique<AautoFisher>(afId);
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
	spawnLoc = { 557, 506 };
	houseLoc = { 1670, -870 };
	bankSellLoc = vector(710, 470);

	setWorldChangeLoc(worldChangeLoc);

	mapImg = std::make_unique<Image>("images/worlds/demo/map.png", vector{ 0, 0 }, true);
	for (int i = 0; i < 19; i++)
		mapAnimList.push_back("images/worlds/world1/map" + std::to_string(i + 1) + ".png");
	//ship = std::make_unique<Aship>(vector{ 336.f, 470.f });

	fishBin = std::make_unique<FishBin>(vector(662.f, 472.f));

	// npcs
	if (SaveData::saveData.progressionData.at(90u).level)
		fishTransporter = std::make_unique<AfishTransporter>(vector(710, 470));

	fisherman = std::make_unique<Afisherman>(vector(792, 533.f));
	merchant = std::make_unique<Amerchant>(vector(905.f, 300.f));
	mechanic = std::make_unique<Amechanic>(vector(1285.f, 578.f));
	petSeller = std::make_unique<ApetSeller>(vector(1010.f, 772.f));
	//sailor = std::make_unique<Asailor>(vector{ 510, 544 });
	//atm = std::make_unique<Aatm>(vector{ 775.f, 736.f });
	//scuba = std::make_unique<Ascuba>(vector{ 749.f, 412.f });

	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/fishermanHouse.png", vector(655, 486), vector(10, 34), vector(221, 72)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/canopyBoxes1.png", vector(599, 482), vector(0, 9), vector(62, 17)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/canopyBoxes2.png", vector(599, 482), vector(4, 18), vector(43, 24)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/canopyPole.png", vector(660, 472), vector(0, -2), vector(19, 7)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/mechanicShop.png", vector(1185, 631), vector(0, 26), vector(165, 46)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/picnicTable.png", vector(1284, 515), vector(0, 23), vector(97, 18)));
	buildingList.push_back(std::make_unique<Abuilding>("images/worlds/Demo/bridgePole.png", vector(801, 731), vector(0, 9), vector(20, 0)));

	std::vector<vector> rockLocs = {};
	for (int i = 0; i < rockLocs.size(); i++)
		buildingList.push_back(std::make_unique<Arock>(rockLocs[i]));

	// make trees
	std::vector<vector> treeLocs = { { 611, 1183 }, { 702, 1182 }, { 492, 1163 }, { 649, 1149 }, { 394, 1143 }, { 750, 1138 }, { 561, 1138 }, { 822, 1115 }, { 466, 1094 },
									 { 690, 1092 }, { 303, 1085 }, { 600, 1073 }, { 782, 1071 }, { 896, 1047 }, { 379, 1043 }, { 702, 1035 }, { 525, 1032 }, { 218, 1026 },
									 { 972, 1020 }, { 818, 1015 }, { 599, 1004 }, { 296, 995 }, { 424, 988 }, { 661, 984 }, { 876, 975 }, { 744, 970 }, { 507, 960 }, { 574, 933 },
									 { 673, 912 }, { 1138, 908 }, { 1166, 887 }, { 1102, 880 }, { 1431, 760 } };

	std::vector<vector> bushLocs = { {538, 1050},{857, 1028},{652, 1026},{468, 955},{1012, 946},{931, 933},{449, 895},{831, 886},{343, 879},{613, 848},{1210, 846},{1333, 796},
									 {1372, 777},{1395, 756},{1354, 715},{1461, 709},{1492, 698},{1180, 695},{1519, 688},{1570, 668},{1590, 656},{1618, 646},{1484, 585},{1457, 571},
									 {1337, 511},{1300, 510},{1318, 498} };

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

	waterImg = std::make_unique<Image>("images/worlds/demo/underwater.png", vector{ 0, 0 }, true);

	// pole list
	std::vector<std::pair<std::string, vector>> poleLocs = { { "1", { 695, 428 } }, { "4", { 767, 463 } }, { "3", { 871, 513 } }, { "2", { 919, 537 } }, { "2", { 617, 455 } },
															 { "1", { 560, 484 } }, { "1", { 550, 395 } }, { "2", { 465, 408 } }, { "2", { 457, 348 } }, { "1", { 356, 353 } },
															 { "2", { 349, 293 } }, { "1", { 231, 290 } }, { "1", { 321, 246 } }, { "2", { 416, 256 } }, { "2", { 410, 202 } },
															 { "1", { 495, 159 } }, { "1", { 495, 217 } }, { "2", { 621, 222 } }, { "1", { 594, 264 } }, { "2", { 705, 264 } },
															 { "1", { 694, 314 } }, { "2", { 778, 299 } }, { "2", { 822, 278 } }, { "2", { 847, 290 } }, { "2", { 805, 347 } },
															 { "1", { 866, 379 } } };
	
	for (int i = 0; i < poleLocs.size(); i++) {
		std::unique_ptr<Image> poleImg = std::make_unique<Image>("images/worlds/demo/dockPole" + poleLocs[i].first + ".png", poleLocs[i].second, true);
		poleList.push_back(std::move(poleImg));
	}

	inFront = std::make_unique<Image>("images/worlds/demo/inFront.png", vector(624.f, 448.f), true);

	grass = std::make_unique<Grass>();
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
