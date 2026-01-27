#include "merchant.h"
#include "merchantWidget.h"
#include "main.h"

#include "debugger.h"

Amerchant::Amerchant(vector loc) : npc(loc) {
	setup("merchant");
	merchantWidget = std::make_unique<UmerchantWidget>(nullptr, this);

	discovered = &SaveData::saveData.npcSave.at(Scene::GetCurrWorldId()).merchantDiscovered;
}

Amerchant::~Amerchant() {

}

void Amerchant::click() {
	*discovered = true;
	merchantWidget->addToViewport(nullptr);
}