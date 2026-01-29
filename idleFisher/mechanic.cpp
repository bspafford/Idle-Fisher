#include "mechanic.h"
#include "main.h"
#include "mechanicWidget.h"

#include "debugger.h"

Amechanic::Amechanic(vector loc) : npc(loc) {
	setup("mechanic");
	mechanicWidget = std::make_unique<UmechanicWidget>(nullptr, this);

	discovered = &SaveData::saveData.npcSave[Scene::GetCurrWorldId()].mechanicDiscovered;
}

Amechanic::~Amechanic() {

}

void Amechanic::click() {
	*discovered = true;
	mechanicWidget->addToViewport(nullptr);
}