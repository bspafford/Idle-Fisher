#include "Ambience.h"
#include "character.h"

Ambience::Ambience() {
	minTime = 5.f;
	maxTime = 20.f;

	beachAudio = std::make_unique<Audio>("ambience/beachAmbience.wav", AudioType::Ambient);
	creatureSounds = std::make_unique<Audio>("ambience/bird.wav", AudioType::Ambient, vector(0, 0));
	beachAudio->Play(true);

	creatureTimer = CreateDeferred<Timer>();
	creatureTimer->addCallback(this, &Ambience::CreatureCallback);
	creatureTimer->start(math::randRange(minTime, maxTime));
}

void Ambience::Update() {
	creatureSounds->SetLoc(GetCharacter()->getCharLoc() + randDir * 200.f);
}

void Ambience::CreatureCallback() {
	// get a random direction
	randDir = math::normalize(vector(math::randRange(-1.f, 1.f), math::randRange(-1.f, 1.f)));

	std::vector<std::string> creatureList = { "bird.wav", "frog.wav", "seagull.wav", "seagull1.wav", "seagull2.wav" };
	int idx = math::randRange(0.f, creatureList.size() - 1.f);
	creatureSounds->SetAudio("ambience/" + creatureList[idx]);
	creatureSounds->Play();
	creatureTimer->start(math::randRange(minTime, maxTime));
}