#pragma once

#include "Audio.h"
#include "timer.h"

class Ambience {
public:
	Ambience();

	void Update();

private:
	void CreatureCallback();

	std::unique_ptr<Audio> beachAudio;
	std::unique_ptr<Audio> creatureSounds;

	DeferredPtr<Timer> creatureTimer;

	vector randDir;
	float minTime;
	float maxTime;
};