#include "petSeller.h"
#include "main.h"

#include "debugger.h"

ApetSeller::ApetSeller(vector loc) : npc(loc) {
	setup("petSeller");
	widget->setup(SaveData::data.modifierData, SaveData::orderedData.petData, SaveData::saveData.progressionData);

	discovered = &SaveData::saveData.npcSave[Scene::GetCurrWorldId()].petSellerDiscovered;
}

void ApetSeller::click() {
	*discovered = true;

	__super::click();
}