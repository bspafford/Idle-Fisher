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
#include "upgrades.h"

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

	for (uint32_t worldId : SaveData::orderedData.worldData) {
		ProgressionNode& worldData = SaveData::data.progressionData.at(worldId);
		SaveEntry& worldSaveData = SaveData::saveData.progressionData.at(worldId);

		std::string textString = "";
		if (worldSaveData.level)
			textString = worldData.name;
		else
			textString = shortNumbers::convert2Short(Upgrades::GetPrice(worldData, worldSaveData));
			
		std::unique_ptr<Ubutton> button = std::make_unique<Ubutton>(this, "widget/maps/x.png", 21, 22, 1, worldButtonLoc[0], false, false);
		std::unique_ptr<text> nameText = std::make_unique<text>(this, textString, "straight", worldButtonLoc[0] + vector{ worldButtons.at(worldId).first->getSize().x / 2, 25}, false, false, TEXT_ALIGN_CENTER);
		worldButtons.insert({ worldId, std::pair(std::move(button), std::move(nameText)) });
	}

	for (auto& [worldId, locs] : worldButtonLoc) {
		std::unique_ptr<Image> line = std::make_unique<Image>("images/widget/maps/mapLine" + std::to_string(worldId) + ".png", vector{ 0, 0 }, false);
		worldLines.insert({ worldId, std::move(line) });
	}

	if (worldButtons.size() >= 10) {
		worldButtons.at(53u).first->addCallback(this, &Umap::openWorld1);
		worldButtons.at(54u).first->addCallback(this, &Umap::openWorld2);
		worldButtons.at(55u).first->addCallback(this, &Umap::openWorld3);
		worldButtons.at(56u).first->addCallback(this, &Umap::openWorld4);
		worldButtons.at(57u).first->addCallback(this, &Umap::openWorld5);
		worldButtons.at(58u).first->addCallback(this, &Umap::openWorld6);
		worldButtons.at(59u).first->addCallback(this, &Umap::openWorld7);
		worldButtons.at(60u).first->addCallback(this, &Umap::openWorld8);
		worldButtons.at(61u).first->addCallback(this, &Umap::openWorld9);
		worldButtons.at(62u).first->addCallback(this, &Umap::openWorld10);
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
	for (uint32_t worldId : SaveData::orderedData.worldData) {
		SaveEntry& worldSaveData = SaveData::saveData.progressionData.at(worldId);

		if (!worldSaveData.level)
			break;

		worldLines[lineIndex]->draw(shaderProgram);

		lineIndex++;
	}

	int currWorldId = Scene::GetCurrWorldId();
	for (uint32_t worldId : SaveData::orderedData.worldData) {
		SaveEntry& worldSaveData = SaveData::saveData.progressionData.at(worldId);

		auto& [worldButton, worldText] = worldButtons.at(worldId);
		if (worldId != currWorldId) {
			worldButton->draw(shaderProgram);
			worldText->draw(shaderProgram);

			if (!worldSaveData.level) // stop drawing if not unlocked
				break;
		} else { // draws boat at current world map loc
			hereBoat->setLoc(worldButton->getLoc());
			hereBoat->draw(shaderProgram);
			hereText->setLoc(worldText->getLoc());
			hereText->draw(shaderProgram);
		}
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

	for (uint32_t worldId : SaveData::orderedData.worldData) {
		auto& [worldButton, worldText] = worldButtons.at(worldId);

		worldButton->setLoc(loc + worldButtonLoc.at(worldId));
		worldText->setLoc(loc + worldButtonLoc.at(worldId) + vector{worldButton->getSize().x / 2.f, 25.f});
	}

	for (int i = 0; i < worldLines.size(); i++)
		worldLines[i]->setLoc(loc + worldLineLoc[i]);
}

void Umap::openLevel(uint32_t levelId) {
	ProgressionNode* worldData = &SaveData::data.progressionData.at(levelId);
	SaveEntry& saveWorld = SaveData::saveData.progressionData.at(levelId);

	if (saveWorld.level) { // already unlocked
		Scene::openLevel(levelId);
	} else if (Upgrades::LevelUp(levelId, Stat::None)) {
		// update the map
		worldButtons.at(levelId).second->setText(worldData->name);
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
	uint32_t worldId = Scene::GetCurrWorldId();
	if (worldId == 0u)
		return;
	vector worldButtonLoc = this->worldButtonLoc.at(worldId);
	vector centerScreen = (stuff::screenSize / stuff::pixelSize - worldButtons.at(worldId).first->getSize()) / 2.f;
	vector mapLoc = centerScreen - worldButtonLoc;
	vector clamped = vector::clamp(mapLoc, size - mapImg->getSize() + ogLoc, ogLoc);
	setLocs(clamped);
}