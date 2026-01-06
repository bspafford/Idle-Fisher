#include "house.h"
#include "Scene.h"
#include "button.h"
#include "worlds.h"

Ahouse::Ahouse(vector loc) : Abuilding(loc) {
	buildingImg = std::make_unique<Image>("images/house.png", loc, true);
	door = std::make_unique<Ubutton>(nullptr, "houseDoor.png", 23, 62, 1, loc + vector{ 34, 19 }, true, true);
	door->addCallback(this, &Ahouse::enterVault);

	setupCollision({ loc + vector{3, 44}, loc + vector{91, 0}, loc + vector{175, 42}, loc + vector{77, 86} });
}

Ahouse::~Ahouse() {

}

void Ahouse::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	door->draw(shaderProgram);
}

void Ahouse::setLoc(vector loc) {
	__super::setLoc(loc);
	door->setLoc(loc + vector{ 34, 73 });
}

void Ahouse::enterVault() {
	Scene::openLevel("vault");
}