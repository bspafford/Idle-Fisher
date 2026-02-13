#include "rain.h"
#include "main.h"
#include "timer.h"
#include "animation.h"
#include "saveData.h"

#include "debugger.h"

Arain::Arain() {
	rainDrop = std::make_unique<Image>("images/rain.png", vector{ 0, 0 }, true);
	rainTimer = CreateDeferred<Timer>();
	rainTimer->addCallback(this, &Arain::UpdateRain);

	std::string path = "images/misc/rainDropAnim/rainDropAnim";
	for (int i = 0; i < 8; i++)
		animImgList.push_back(path + std::to_string(i) + ".png");

	darkenScreenTimer = CreateDeferred<Timer>();
	darkenScreenTimer->addUpdateCallback(this, &Arain::DarkenScreen);

	darkBackground = std::make_unique<URectangle>(nullptr, vector(0, 0), stuff::screenSize / stuff::pixelSize, false, glm::vec4(0));
}

void Arain::Draw(Shader* shaderProgram) {
	if (!SaveData::settingsData.showRain)
		return;

	darkBackground->setSize(stuff::screenSize / stuff::pixelSize);
	darkBackground->draw(shaderProgram);

	for (int i = 0; i < animList.size(); i++) {
		animList[i]->draw(shaderProgram);
	}

	for (int i = 0; i < locs.size(); i++) {
		rainDrop->setLoc(locs[i]);
		rainDrop->draw(shaderProgram);
	}
}

void Arain::Start() {
	raining = true;
	isStopped = false;

	darkenScreenTimer->start(1);

	int rainNum = 100;
	vector screenSize = stuff::screenSize / stuff::pixelSize;
	vector minWorldLoc = math::screenToWorld(-screenSize); // get off screen by screen size
	vector maxWorldLoc = math::screenToWorld(screenSize * 2.f); // get off screen by screen size
	for (int i = 0; i < rainNum; i++) {
		// set updated random position above screen
		float randX = math::randRange(minWorldLoc.x, maxWorldLoc.x);
		float randY = math::randRange(minWorldLoc.y, maxWorldLoc.y);
		locs.push_back(vector(randX, randY));
		// sets updated ground location
		deathLoc.push_back(math::randRange(minWorldLoc.y, maxWorldLoc.y));
	}

	rainTimer->start(.1);
}

void Arain::Stop() {
	raining = false;
	isStopped = true;

	darkenScreenTimer->start(1);
}

void Arain::UpdateRain() {
	// remove all finished animations
	for (int i = animList.size() - 1; i >= 0; --i) {
		if (animList[i]->IsFinished())
			animList.erase(animList.begin() + i);
	}

	vector screenSize = stuff::screenSize / stuff::pixelSize;
	vector minWorldLoc = math::screenToWorld(-screenSize); // get off screen by screen size
	for (int i = locs.size() - 1; i >= 0; --i) {
		// down 5 left 1
		locs[i] -= vector(3, 15);

		// if offscreen or hit ground
		if (locs[i].y < minWorldLoc.y || locs[i].y <= deathLoc[i]) {
			// create water drop animation
			std::unordered_map<std::string, animDataStruct> animData;
			animData.insert({ "anim", animDataStruct({0, 0}, {7, 0}, false) });
			std::unique_ptr<animation> anim = std::make_unique<animation>("misc/rainDropAnim/rainDropAnim.png", 17, 9, animData, true, locs[i]);
			anim->setAnimation("anim");
			anim->start();
			animList.push_back(std::move(anim));
			
			if (!isStopped) {
				vector maxWorldLoc = math::screenToWorld(screenSize * 2.f); // get off screen by screen size
				// set updated random position above screen
				float randX = math::randRange(minWorldLoc.x, maxWorldLoc.x);
				float randY = math::randRange(minWorldLoc.y, maxWorldLoc.y);
				locs[i] = vector(randX, randY);
				// sets updated ground location
				deathLoc[i] = math::randRange(minWorldLoc.y, maxWorldLoc.y);
			} else { // if stopped
				// remove the raindrop from the list
				locs.erase(locs.begin() + i);
				deathLoc.erase(deathLoc.begin() + i);
			}
		}
	}

	rainTimer->start(0.1f);
}

void Arain::DarkenScreen() {
	if (IsRaining()) // get darker
		darkBackground->setColor(glm::vec4(0, 0, 0, darkenScreenTimer->getPercent() * 0.25f));
	else // brighten up
		darkBackground->setColor(glm::vec4(0, 0, 0, (1 - darkenScreenTimer->getPercent()) * 0.25f));
}

bool Arain::IsRaining() {
	return raining;
}