#include "petSeller.h"
#include "main.h"

#include "debugger.h"

ApetSeller::ApetSeller(vector loc) : npc(loc) {
	setup("petSeller");
	widget->setup(SaveData::data.petData, SaveData::saveData.petList);

	discovered = &SaveData::saveData.npcSave.at(Scene::GetCurrWorldId()).petSellerDiscovered;
}

void ApetSeller::click() {
	*discovered = true;

	__super::click();
}