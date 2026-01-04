#include "scuba.h"
#include "main.h"

#include "debugger.h"

Ascuba::Ascuba(vector loc) : npc(loc) {
	setup("scuba");

	rebirthWidget = std::make_unique<UrebirthWidget>(nullptr);

	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	discovered = &SaveData::saveData.npcSave[worldId].scubaDiscovered;
}

void Ascuba::click() {
	*discovered = true;

	rebirthWidget->addToViewport(nullptr);
}