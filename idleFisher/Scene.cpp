#include "Scene.h"
#include "main.h"
#include "worlds.h"
#include "saveData.h"
#include "collision.h"
#include "Texture.h"
#include "AStar.h"
#include "GPULoadCollector.h"
#include "Audio.h"
#include "Input.h"

#include "camera.h"
#include "character.h"
#include "widget.h"
#include "upgrades.h"
#include "achievementBuffs.h"
#include "achievement.h"
#include "pet.h"
#include "textureManager.h"

#include "debugger.h"

void Scene::Init() {
	Scene::camera = std::make_unique<Camera>(glm::vec3(-55, 50, -350));
	CreateShaders();
}

void Scene::Destructor() {
	delete shaderProgram;
	delete shadowMapProgram;
	delete twoDShader;
	delete twoDWaterShader;
	delete circleShader;
	delete blurShader;
	delete fishingLineShader;
}

void Scene::openLevel(std::string _worldName, WorldLoc _worldChangeLoc, bool _overrideIfInWorld, bool _isStartup) {
	std::unique_lock lock(mtx);

	isStartup = _isStartup;

	worldName = _worldName;
	worldChangeLoc = _worldChangeLoc;
	overrideIfInWorld = _overrideIfInWorld;
	loadWorld = true;

	if (!loadingScreen)
		loadingScreen = std::make_unique<LoadingScreen>(nullptr);
}

void Scene::draw(Shader* shaderProgram) {
	std::unique_lock lock(mtx);

	if (loadingTexturesDone && !hasLoadedGPUData) {
		std::unique_lock lock(mtx);
		hasLoadedGPUData = true;
		GPULoadCollector::LoadAllGPUData();
		Main::setupWidgets();
		waitToUploadGPUdata = false;
		cv.notify_one();
	}

	// first frame after finished loading
	if (loadingDone && !hasFinishedLoading) {
		hasFinishedLoading = true;
		finishedLoading();
		// draw loading screen while loading
	} else if (!loadingDone) {
		loadingScreen->draw(shaderProgram);
	}

	// draw game loop after loaded
	if (loadingDone) {
		std::string currWorldName = Scene::getCurrWorldName();
		if (titleScreen::currTitleScreen && currWorldName == "titleScreen") {
			titleScreen::currTitleScreen->draw(shaderProgram);
		} else if (currWorldName == "vault") {
			vaultWorld::draw(shaderProgram);
		} else if (currWorldName == "rebirth") {
			rebirthWorld::draw(shaderProgram);
		} else if (world::currWorld)
			world::currWorld->draw(shaderProgram);
	}
}

void Scene::openLevelThread(std::string worldName, WorldLoc worldChangeLoc, bool overrideIfInWorld) {
	std::unique_lock lock(mtx);

	std::cout << "starting to load world!\n";

	// a wait to make sure the textures get created properly before loading the world
	if (waitToUploadGPUdata && worldName != "titleScreen") {
		std::unique_lock lock(cvMtx);
		cv.wait(lock, [] { return waitToUploadGPUdata == false; });
	}

	GPULoadCollector::open();

	if (isStartup)
		StartSetup();

	if (worldName == "vault" && currWorldName != "vault")
		prevWorld = currWorldName;
	currWorldName = worldName;
	SaveData::saveData.currWorld = currWorldName;
	if (widget::getCurrWidget())
		widget::getCurrWidget()->removeFromViewport();

	collision::LoadWorldsCollision(worldName);
	// deconstruct worlds
	Texture::deleteCache();
	AStar::Deconstructor();
	vaultWorld::deconstructor();
	rebirthWorld::deconstructor();
	world::currWorld = nullptr;
	titleScreen::currTitleScreen = nullptr;
	
	if (currWorldName == "titleScreen") {
		titleScreen::currTitleScreen = std::make_unique<titleScreen>();
	} else if (currWorldName == "vault") {
		vaultWorld::vaultWorld();
		vaultWorld::start();
	} else if (currWorldName == "rebirth") {
		rebirthWorld::rebirthWorld();
		rebirthWorld::start();
	} else if (currWorldName == "world1") {
		world::currWorld = std::make_unique<world1>(worldChangeLoc);
	} else if (currWorldName == "world2") {
		world::currWorld = std::make_unique<world2>(worldChangeLoc);
	} else if (currWorldName == "world3") {
		world::currWorld = std::make_unique<world3>(worldChangeLoc);
	} else if (currWorldName == "world4") {
		world::currWorld = std::make_unique<world4>(worldChangeLoc);
	} else if (currWorldName == "world5") {
		world::currWorld = std::make_unique<world5>(worldChangeLoc);
	} else if (currWorldName == "world6") {
		world::currWorld = std::make_unique<world6>(worldChangeLoc);
	} else if (currWorldName == "world7") {
		world::currWorld = std::make_unique<world7>(worldChangeLoc);
	} else if (currWorldName == "world8") {
		world::currWorld = std::make_unique<world8>(worldChangeLoc);
	} else if (currWorldName == "world9") {
		world::currWorld = std::make_unique<world9>(worldChangeLoc);
	} else if (currWorldName == "world10") {
		world::currWorld = std::make_unique<world10>(worldChangeLoc);
	}

	if (isStartup)
		FinishSetup();

	AStar::init();

	loadingDone = true;

	std::cout << "finished loading world!\n";
}

void Scene::finishedLoading() {
	GPULoadCollector::LoadAllGPUData();
	if (world::currWorld)
		world::currWorld->start();
	if (titleScreen::currTitleScreen)
		titleScreen::currTitleScreen->start();

	if (!Main::settingsWidget) // load settings widget for titleScreen
		Main::settingsWidget = std::make_unique<Usettings>(nullptr);

	if (!textureManager::GetTexturesLoaded()) {
		std::thread loader(&Scene::LoadTextures);
		loader.detach();
	}
}

void Scene::LoadTextures() {
	std::cout << "start loading textures!\n";
	Audio::LoadData();
	textureManager::LoadTextures();
	loadingTexturesDone = true;
	std::cout << "finished loading textures!\n";
}

int Scene::getWorldIndexFromName(std::string worldName) {
	std::unique_lock lock(mtx);

	for (int i = 0; i < SaveData::data.worldData.size(); i++)
		if (SaveData::data.worldData[i].worldName == worldName)
			return i;
	return -1;
}

std::string Scene::getPrevWorldName() {
	return prevWorld;
}

std::string Scene::getCurrWorldName() {
	std::unique_lock lock(mtx);
	return currWorldName;
}

bool Scene::isLoading() {
	std::unique_lock lock(mtx);
	return !loadingDone;
}

void Scene::deferredChangeWorld() {
	if (!loadWorld)
		return;

	loadWorld = false;

	// returns if the world is already open
	if (!overrideIfInWorld && worldName == currWorldName)
		return;

	hasFinishedLoading = false;
	loadingDone = false;

	std::thread loader(&Scene::openLevelThread, worldName, worldChangeLoc, overrideIfInWorld);
	loader.detach();
}

void Scene::CreateShaders() {
	Shader::Init();
	shaderProgram = new Shader("default.vert", "default.frag");
	shadowMapProgram = new Shader("shadowMap.vert", "shadowMap.frag");
	twoDShader = new Shader("2dShader.vert", "2dShader.frag");
	twoDWaterShader = new Shader("2dWaterShader.vert", "2dWaterShader.frag");
	circleShader = new Shader("2dShader.vert", "circleShader.frag");
	blurShader = new Shader("blurShader.vert", "blurShader.frag");
	fishingLineShader = new Shader("2dShader.vert", "fishingLineShader.frag");
	Shader::CleanUp();

	updateShaders(0);
}

void Scene::StartSetup() {
	Input::Init();
	SaveData::LoadData();

	character = std::make_unique<Acharacter>();
}

void Scene::FinishSetup() {
	SaveData::load();
	upgrades::init();
	achievementBuffs::init();
	if (SaveData::saveData.equippedPet.id != -1)
		pet = std::make_unique<Apet>(&SaveData::saveData.equippedPet, vector{ 400, -200 });
	achievement::createAchievementList();
}

void Scene::updateShaders(float deltaTime) {
	std::unique_lock lock(mtx);
	// set water movement
	waveFactor += waveSpeed * deltaTime;
	if (waveFactor >= 1.f) // loop if hit 1
		waveFactor -= 1.f;

	glm::mat4 camProj = camera->getProjectionMat();
	glm::vec3 camPos = camera->GetPosition();

	twoDShader->Activate();
	twoDShader->setMat4("projection", camProj);
	twoDShader->setVec2("playerPos", camPos);
	twoDShader->setFloat("pixelSize", stuff::pixelSize);

	fishingLineShader->Activate();
	fishingLineShader->setMat4("projection", camProj);
	fishingLineShader->setVec2("playerPos", camPos);
	fishingLineShader->setFloat("pixelSize", stuff::pixelSize);

	twoDWaterShader->Activate();
	twoDWaterShader->setFloat("moveFactor", waveFactor);
	twoDWaterShader->setMat4("projection", camProj);
	twoDWaterShader->setVec2("playerPos", camPos);
	twoDWaterShader->setFloat("pixelSize", stuff::pixelSize);

	blurShader->Activate();
	blurShader->setMat4("projection", GetMainCamera()->getProjectionMat());
	blurShader->setVec2("playerPos", camPos);
	blurShader->setFloat("pixelSize", stuff::pixelSize);
}