#include "map.h"
#include "main.h"
#include "Input.h"
#include "button.h"
#include "text.h"
#include "saveData.h"
#include "currencyWidget.h"
#include "shortNumbers.h"
#include "animation.h"
#include "sailorWidget.h"
#include "ScissorTest.h"

#include "debugger.h"

Umap::Umap(UsailorWidget* parent, vector mapSize) : widget(parent) {
	this->sailorWidgetParent = parent;
	setSize(mapSize);

	mapImg = std::make_unique<Image>("images/sailorMap.png", vector{ 0.f, 0.f }, false);

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "anim", animDataStruct({0, 0}, {9, 0}, true, 0.333f) });
	hereBoat = std::make_unique<animation>("widget/hereBoat.png", 22, 22, animData, false, vector{ 0, 0 });
	hereBoat->setAnimation("anim");
	hereBoat->start();
	hereText = std::make_unique<text>(this, "Here", "straight", vector{ 0.f, 0.f }, false, false, TEXT_ALIGN_CENTER);

	for (auto& [worldId, worldSaveData] : SaveData::saveData.worldList) {
		FworldStruct& worldData = SaveData::data.worldData.at(worldId);
		int worldIndex = Scene::GetWorldIndex();

		std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "widget/maps/x.png", 21, 22, 1, worldButtonLoc[worldIndex], false, false);
		button->setParent(this);
		worldButtons.push_back(std::move(button));
		

		std::string textString = "";
		if (worldSaveData.level)
			textString = worldData.name;
		else
			textString = shortNumbers::convert2Short(worldData.currencyNum);
		if (worldButtons.size() > worldIndex)
			worldNames.push_back(std::make_unique<text>(this, textString, "straight", worldButtonLoc[worldIndex] + vector{ worldButtons[worldIndex]->getSize().x / 2, 25 }, false, false, TEXT_ALIGN_CENTER));
	}

	for (int i = 0; i < worldButtonLoc.size() - 1; i++) {
		std::unique_ptr<Image> line = std::make_unique<Image>("images/widget/maps/mapLine" + std::to_string(i + 1) + "-" + std::to_string(i + 2) + ".png", vector{ 0, 0 }, false);
		worldLines.push_back(std::move(line));
	}

	if (worldButtons.size() >= 10) {
		worldButtons[0]->addCallback(this, &Umap::openWorld1);
		worldButtons[1]->addCallback(this, &Umap::openWorld2);
		worldButtons[2]->addCallback(this, &Umap::openWorld3);
		worldButtons[3]->addCallback(this, &Umap::openWorld4);
		worldButtons[4]->addCallback(this, &Umap::openWorld5);
		worldButtons[5]->addCallback(this, &Umap::openWorld6);
		worldButtons[6]->addCallback(this, &Umap::openWorld7);
		worldButtons[7]->addCallback(this, &Umap::openWorld8);
		worldButtons[8]->addCallback(this, &Umap::openWorld9);
		worldButtons[9]->addCallback(this, &Umap::openWorld10);
	}

	setLocs(mapImg->getLoc()); // updates starting pos
}

void Umap::draw(Shader* shaderProgram) {
	ScissorTest::Enable(ogLoc, getSize());

	mouseDown = Input::getMouseButtonHeld(MOUSE_BUTTON_LEFT);
	vector mousePos = Input::getMousePos();
	bool mouseStartX = mousePos.x >= ogLoc.x && mousePos.x <= ogLoc.x + size.x;
	bool mouseStartY = mousePos.y >= ogLoc.y && mousePos.y <= ogLoc.y + size.y;
	if ((mouseStartX && mouseStartY) || movingMap) {
		setCursorHoverIcon(CURSOR_HOVER);
		IHoverable::setHoveredItem(this);
	}

	// first frame mouse button goes down
	if (mouseDown && !prevMouseDown) {
		// make sure mouse starts inside of the map view
		if (mouseStartX && mouseStartY) {
			movingMap = true;
			imgStartPos = mapImg->getLoc();
			mouseStartPos = mousePos;
		}
		// first frame mouse button was released
	} else if (!mouseDown && prevMouseDown)
		movingMap = false;

	if (mouseDown)
		moveMap();

	prevMouseDown = mouseDown;

	if (mapImg)
		mapImg->draw(shaderProgram);

	int lineIndex = 0;
	for (auto& [worldId, worldData] : SaveData::saveData.worldList) {
		if (!worldData.level)
			break;

		worldLines[lineIndex]->draw(shaderProgram);

		lineIndex++;
	}

	int worldIndex = 0;
	for (auto& [worldId, worldData] : SaveData::saveData.worldList) {
		if (worldIndex != Scene::GetWorldIndex()) {
			worldButtons[worldIndex]->draw(shaderProgram);
			worldNames[worldIndex]->draw(shaderProgram);

			if (!worldData.level)
				break;
		} else { // draws boat at current world map loc
			hereBoat->setLoc(worldButtons[worldIndex]->getLoc());
			hereBoat->draw(shaderProgram);
			hereText->setLoc(worldNames[worldIndex]->getLoc());
			hereText->draw(shaderProgram);
		}

		worldIndex++;
	}

	ScissorTest::Disable();
}

void Umap::moveMap() {
	if (!movingMap)
		return;

	vector diff = mouseStartPos - imgStartPos;
	vector newLoc = Input::getMousePos() - diff;

	vector clamped = vector::clamp(newLoc, size - mapImg->getSize() + ogLoc, ogLoc);
	setLocs(clamped);
}

void Umap::setLocs(vector loc) {
	this->ogLoc = (stuff::screenSize / stuff::pixelSize - size) / 2.f;

	mapImg->setLoc(loc);

	for (int i = 0; i < worldButtons.size(); i++) {
		worldButtons[i]->setLoc(loc + worldButtonLoc[i]);
		worldNames[i]->setLoc(loc + worldButtonLoc[i] + vector{ worldButtons[i]->getSize().x / 2.f, 25.f });
	}

	for (int i = 0; i < worldLines.size(); i++)
		worldLines[i]->setLoc(loc + worldLineLoc[i]);
}

void Umap::openLevel(uint32_t levelId) {
	// world to id
	FworldStruct* world = nullptr;
	for (auto& [worldId, worldData] : SaveData::data.worldData) {
		if (worldData.id == levelId) {
			world = &worldData;
			break;
		}
	}

	SaveEntry* saveWorld = &SaveData::saveData.worldList.at(world->id);
	if (!world || !saveWorld)
		return;

	if (saveWorld->level) { // already unlocked
		Scene::openLevel(levelId);
	} else if (world->currencyNum <= SaveData::saveData.currencyList[world->currencyId].numOwned) { // can afford
		SaveData::saveData.currencyList[world->currencyId].numOwned -= world->currencyNum;
		saveWorld->level = true;

		// update the map
		worldNames[Scene::GetWorldIndex(world->id)]->setText(world->name);
		Main::currencyWidget->updateList();
	}

}

void Umap::setLoc(vector loc) {
	__super::setLoc(loc);
	this->loc = loc;
}
	
vector Umap::getLoc() {
	return mapImg->getLoc();
}

void Umap::SetCurrWorldToCenter() {
	int worldIndex = Scene::GetWorldIndex();
	if (worldIndex == -1)
		return;
	vector worldButtonLoc = this->worldButtonLoc[worldIndex];
	vector centerScreen = (stuff::screenSize / stuff::pixelSize - worldButtons[0]->getSize()) / 2.f;
	vector mapLoc = centerScreen - worldButtonLoc;
	vector clamped = vector::clamp(mapLoc, size - mapImg->getSize() + ogLoc, ogLoc);
	setLocs(clamped);
}