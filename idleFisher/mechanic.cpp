#include "mechanic.h"
#include "main.h"
#include "mechanicWidget.h"

#include "debugger.h"

Amechanic::Amechanic(vector loc) : npc(loc) {
	setup("mechanic");
	mechanicWidget = std::make_unique<UmechanicWidget>(nullptr, this);

	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	discovered = &SaveData::saveData.npcSave[worldId].mechanicDiscovered;
}

Amechanic::~Amechanic() {

}

void Amechanic::click() {
	*discovered = true;
	mechanicWidget->addToViewport(nullptr);
}