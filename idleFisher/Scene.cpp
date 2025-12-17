#include "Scene.h"
#include "worlds.h"
#include "saveData.h"
#include "widget.h"
#include "collision.h"
#include "Texture.h"
#include "AStar.h"
#include "timer.h"
#include "loadingScreen.h"
#include "GPULoadCollector.h"
#include "Image.h"
#include "animation.h"
#include "text.h"

#include "debugger.h"

void Scene::openLevel(std::string worldName1, int worldChangeLoc1, bool overrideIfInWorld1) {
	worldName = worldName1;
	worldChangeLoc = worldChangeLoc1;
	overrideIfInWorld = overrideIfInWorld1;
	loadWorld = true;

	if (!loadingScreen)
		loadingScreen = std::make_unique<LoadingScreen>(nullptr);
}

void Scene::draw(Shader* shaderProgram) {
	// first frame after finished loading
	if (loadingDone && !hasFinishedLoading) {
		hasFinishedLoading = true;
		finishedLoading();
	// draw loading screen while loading
	} else if (!loadingDone && world::currWorld) {
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

void Scene::openLevelThread(std::string worldName, int worldChangeLoc, bool overrideIfInWorld) {
	GPULoadCollector::open();

	if (worldName == "vault" && currWorldName != "vault")
		prevWorld = currWorldName;
	currWorldName = worldName;
	SaveData::saveData.currWorld = currWorldName;
	if (widget::getCurrWidget())
		widget::getCurrWidget()->removeFromViewport();

	collision::getCollisionObjects();
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
	
	AStar::init();
	loadingDone = true;
}

void Scene::finishedLoading() {
	GPULoadCollector::LoadAllGPUData();
	if (world::currWorld)
		world::currWorld->start();
	if (titleScreen::currTitleScreen)
		titleScreen::currTitleScreen->start();
}

int Scene::getWorldIndexFromName(std::string worldName) {
	for (int i = 0; i < SaveData::data.worldData.size(); i++)
		if (SaveData::data.worldData[i].worldName == worldName)
			return i;
	return -1;
}

std::string Scene::getPrevWorldName() {
	return prevWorld;
}

std::string Scene::getCurrWorldName() {
	return currWorldName;
}

bool Scene::isLoading() {
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

	timer::clearInstanceList(true);
	//openLevelThread(worldName, worldChangeLoc, overrideIfInWorld);
	std::thread loader(&Scene::openLevelThread, worldName, worldChangeLoc, overrideIfInWorld);
	loader.detach();
}