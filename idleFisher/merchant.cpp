#include "merchant.h"
#include "merchantWidget.h"
#include "main.h"

#include "debugger.h"

Amerchant::Amerchant(vector loc) : npc(loc) {
	setup("merchant");
	merchantWidget = std::make_unique<UmerchantWidget>(nullptr, this);

	int worldId = Scene::getWorldIndexFromName(Scene::getCurrWorldName());
	discovered = &SaveData::saveData.npcSave[worldId].merchantDiscovered;
}

Amerchant::~Amerchant() {

}

void Amerchant::click() {
	*discovered = true;
	merchantWidget->addToViewport(nullptr);
}