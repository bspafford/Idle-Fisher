#include "mechanic.h"
#include "main.h"
#include "mechanicWidget.h"

#include "debugger.h"

Amechanic::Amechanic(vector loc) : npc(loc) {
	setup("mechanic");
	mechanicWidget = std::make_unique<UmechanicWidget>(nullptr, this);

	discovered = &SaveData::saveData.npcSave[Scene::GetCurrWorldId()].mechanicDiscovered;
	npcAudio = std::make_unique<Audio>("npc1.wav", AudioType::SFX);
}

Amechanic::~Amechanic() {

}

void Amechanic::click() {
	*discovered = true;
	mechanicWidget->addToViewport(nullptr);
	openAudio->Play();
	npcAudio->Play();
}