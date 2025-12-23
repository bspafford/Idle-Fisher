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

#include "debugger.h"

Umap::Umap(UsailorWidget* parent, vector mapSize) : widget(parent) {
	this->sailorWidgetParent = parent;
	setSize(mapSize);

	mapImg = std::make_unique<Image>("./images/sailorMap.png", vector{ 0, 0 }, false); // -1073, -739

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "anim", animDataStruct({0, 0}, {9, 0}, true, 0.333) });
	hereBoat = std::make_unique<animation>("widget/hereBoat.png", 22, 22, animData, false, vector{ 0, 0 });
	hereBoat->setAnimation("anim");
	hereBoat->start();
	hereText = std::make_unique<text>(this, "Here", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	for (int i = 0; i < worldButtonLoc.size(); i++) {
		worldButtonLoc[i] = worldButtonLoc[i] + vector{ 308, 196 };
	}

	for (int i = 0; i < worldButtonLoc.size(); i++) {
		std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "x.png", 21, 22, 1, worldButtonLoc[i] * stuff::pixelSize, false, false);
		button->setParent(this);
		worldButtons.push_back(std::move(button));
		

		std::string textString = "";
		if (SaveData::saveData.worldList[i].unlocked)
			textString = SaveData::data.worldData[i].name;
		else
			textString = shortNumbers::convert2Short(SaveData::data.worldData[i].currencyNum);
		if (worldButtons.size() > i)
			worldNames.push_back(std::make_unique<text>(this, textString, "straight", worldButtonLoc[i] * stuff::pixelSize + vector{ worldButtons[i]->getSize().x / 2, 25 * stuff::pixelSize }, false, false, TEXT_ALIGN_CENTER));
	}

	for (int i = 0; i < worldButtonLoc.size() - 1; i++) {
		std::unique_ptr<Image> line = std::make_unique<Image>("./images/widget/maps/mapLine" + std::to_string(i + 1) + "-" + std::to_string(i + 2) + ".png", vector{ 0, 0 }, false);
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
	glEnable(GL_SCISSOR_TEST);
	glScissor(ogLoc.x, ogLoc.y, getSize().x, getSize().y);

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

	for (int i = 0; i < worldLines.size(); i++) {
		if (!SaveData::saveData.worldList[i].unlocked)
			break;

		worldLines[i]->draw(shaderProgram);
	}

	std::string currWorldName = Scene::getCurrWorldName();
	for (int i = 0; i < worldButtons.size(); i++) {
		std::string currWorld = currWorldName;

		if (i != Scene::getWorldIndexFromName(currWorldName)) {
			worldButtons[i]->draw(shaderProgram);

			if (currWorld != currWorldName) // check if the worlds have changed because of worldButtons might change it
				return;

			worldNames[i]->draw(shaderProgram);

			if (!SaveData::saveData.worldList[i].unlocked)
				break;
		} else { // draws boat at current world map loc
			hereBoat->setLoc(worldButtons[i]->getLoc());
			hereBoat->draw(shaderProgram);
			hereText->setLoc(worldNames[i]->getLoc());
			hereText->draw(shaderProgram);
		}
	}

	glDisable(GL_SCISSOR_TEST);
}

void Umap::moveMap() {
	if (!movingMap)
		return;

	vector diff = mouseStartPos - imgStartPos;
	vector newLoc = Input::getMousePos() - diff;

	float x = math::clamp(newLoc.x, stuff::screenSize.x - mapImg->getSize().x * stuff::pixelSize, 0);
	float y = math::clamp(newLoc.y, stuff::screenSize.y - mapImg->getSize().y * stuff::pixelSize, 0);
	setLocs({ x, y });
}

void Umap::setLocs(vector loc) {
	this->ogLoc = sailorWidgetParent->mapBackground->getLoc() + vector{ 35, 35 } *stuff::pixelSize;
	mapImg->setLoc(loc);

	for (int i = 0; i < worldButtons.size(); i++) {
		worldButtons[i]->setLoc(loc + worldButtonLoc[i] * stuff::pixelSize);

		worldNames[i]->setLoc(loc + worldButtonLoc[i] * stuff::pixelSize + vector{ worldButtons[i]->getSize().x / 2, 25 * stuff::pixelSize });
	}

	for (int i = 0; i < worldLines.size(); i++) {
		worldLines[i]->setLoc(loc + worldLineLoc[i] * stuff::pixelSize);
	}
}

void Umap::openLevel(std::string levelName) {
	// check to see if world unlock
	// if not unlocked
		// buy / unlock it
	// otherwise
		// go there

	// world to id
	FsaveWorldStruct* saveWorld = nullptr;
	FworldStruct* world = nullptr;
	for (int i = 0; i < SaveData::data.worldData.size(); i++) {
		if (SaveData::data.worldData[i].worldName == levelName) {
			world = &SaveData::data.worldData[i];
			saveWorld = &SaveData::saveData.worldList[i];
			break;
		}
	}

	if (!world || !saveWorld)
		return;

	if (saveWorld->unlocked) { // already unlocked
		Scene::openLevel(levelName);
	} else if (world->currencyNum <= SaveData::saveData.currencyList[world->currencyId].numOwned) { // can afford
		SaveData::saveData.currencyList[world->currencyId].numOwned -= world->currencyNum;
		saveWorld->unlocked = true;

		// update the map
		worldNames[world->id]->setText(world->name);
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