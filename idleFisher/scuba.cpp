#include "scuba.h"
#include "main.h"

#include "debugger.h"

Ascuba::Ascuba(vector loc) : npc(loc) {
	setup("scuba");

	rebirthWidget = std::make_unique<UrebirthWidget>(nullptr);

	discovered = &SaveData::saveData.npcSave.at(Scene::GetCurrWorldId()).scubaDiscovered;
}

void Ascuba::click() {
	*discovered = true;

	rebirthWidget->addToViewport(nullptr);
}