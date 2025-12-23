#include "rain.h"
#include "main.h"
#include "timer.h"
#include "animation.h"
#include "saveData.h"

#include "debugger.h"

Arain::Arain() {
	rainDrop = std::make_unique<Image>("./images/rain.png", vector{ 0, 0 }, true);
	rainTimer = std::make_unique<timer>();
	rainTimer->addCallback(this, &Arain::updateRain);

	std::string path = "./images/misc/rainDropAnim/rainDropAnim";
	for (int i = 0; i < 8; i++)
		animImgList.push_back(path + std::to_string(i) + ".png");
}

Arain::~Arain() {

}

void Arain::draw(Shader* shaderProgram) {
	if (!SaveData::settingsData.showRain)
		return;

	for (int i = 0; i < animList.size(); i++) {
		animList[i]->draw(shaderProgram);
	}

	for (int i = 0; i < locs.size(); i++) {
		rainDrop->setLoc(locs[i]);
		rainDrop->draw(shaderProgram);
	}

}

void Arain::start() {
	isStopped = false;

	for (int i = 0; i < 50; i++) {
		float randX;
		vector minWorldLoc = math::screenToWorld(vector{ 0, 0 });
		vector maxWorldLoc = math::screenToWorld(vector{ float(stuff::screenSize.x), float(stuff::screenSize.y) });
		// set updated random position above screen
		float offScreenY = math::screenToWorld({ 0, -rainDrop->getSize().y }).y;
		float doubleOffScreenY = math::screenToWorld({ 0, float(-stuff::screenSize.y)}).y;
		offScreenY = math::randRange(doubleOffScreenY, offScreenY);
		randX = math::randRange(minWorldLoc.x, maxWorldLoc.x);
		locs.push_back(vector{randX, offScreenY});
		// sets updated ground location
		deathLoc.push_back(math::randRange(minWorldLoc.y, maxWorldLoc.y));
	}

	rainTimer->start(.1);
}

void Arain::stop() {
	isStopped = true;
}

void Arain::updateRain() {
	for (int i = locs.size() - 1; i >= 0; i--) {
		// down 5 left 1
		locs[i].x -= 3;
		locs[i].y += 15;

		// if off screen
		float randX;
		vector minWorldLoc = math::screenToWorld(vector{ 0, 0 });
		vector maxWorldLoc = math::screenToWorld(vector{ float(stuff::screenSize.x), float(stuff::screenSize.y) });
		// set updated random position above screen
		if (locs[i].y > maxWorldLoc.y || locs[i].y >= deathLoc[i]) {
			// hit the ground

			std::unordered_map<std::string, animDataStruct> animData;
			animData.insert({ "anim", animDataStruct({0, 0}, {7, 0}, false) });
			std::unique_ptr<animation> anim = std::make_unique<animation>("misc/rainDropAnim/rainDropAnim.png", 17, 9, animData, true, locs[i]);
			anim->setAnimation("anim");
			anim->start();
			anim->addFinishedCallback(this, &Arain::removeAnimList);
			animList.push_back(std::move(anim));
			
			if (!isStopped) {
				// set updated location above screen
				float offScreenY = math::screenToWorld({ 0, -rainDrop->getSize().y }).y;
				offScreenY = math::randRange(offScreenY - 10, offScreenY);
				randX = math::randRange(minWorldLoc.x, maxWorldLoc.x);
				locs[i] = vector{ randX, offScreenY };
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

void Arain::removeAnimList() {
	for (int i = animList.size() - 1; i >= 0; i--) {
		if (animList[i]->IsFinished()) {
			animList[i]->stop();
			animList.erase(animList.begin() + i);
		}
	}
}