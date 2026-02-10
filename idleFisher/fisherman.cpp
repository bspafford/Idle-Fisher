#include "fisherman.h"
#include "fishermanWidget.h"
#include "main.h"

#include "debugger.h"

Afisherman::Afisherman(vector loc) : npc(loc) {
	setup("fisherman");
	fishermanWidget = std::make_unique<UfishermanWidget>(nullptr, this);

	discovered = &SaveData::saveData.npcSave[Scene::GetCurrWorldId()].fishermanDiscovered;
	npcAudio = std::make_unique<Audio>("npc3.wav", AudioType::SFX);
}

void Afisherman::click() {
	// set save data discovered to true
	*discovered = true;
	fishermanWidget->addToViewport(nullptr);
	openAudio->Play();
	npcAudio->Play();
}