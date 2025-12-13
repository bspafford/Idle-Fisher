#include "fishSchool.h"
#include "animation.h"
#include "timer.h"
#include "worlds.h"

#include "debugger.h"

AfishSchool::AfishSchool(vector loc) {
	this->loc = loc;

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "fishSchool", animDataStruct({0, 0}, {6, 0}, true) });
	anim = std::make_unique<animation>("fishSchool/fishSchool.png", 48, 26, animData, true, loc);
	anim->setAnimation("fishSchool");
	anim->start();

	fishNum = math::randRange(1.f, 5.f);
	lifeTimer = std::make_unique<timer>();
	lifeTimer->start(math::randRange(30.f, 60.f));
	lifeTimer->addCallback(this, &AfishSchool::remove);
}

AfishSchool::~AfishSchool() {

}

void AfishSchool::draw(Shader* shaderProgram) {
	anim->draw(shaderProgram);
}

AfishSchool* AfishSchool::pointInSchool(vector worldPoint) {
	// i guess calc if in ellipse
	float h = loc.x + anim->GetCellSize().x / 2.f;
	float k = loc.y + anim->GetCellSize().y / 2.f;
	float a = anim->GetCellSize().x / 2.f;
	float b = anim->GetCellSize().y / 2.f;
	float y = (((worldPoint.x - h) * (worldPoint.x - h)) / (a * a)) + (((worldPoint.y - k) * (worldPoint.y - k)) / (b * b));
	if (y <= 1)
		return this;
	return nullptr;
}

void AfishSchool::removeFishNum() {
	fishNum--;
	if (fishNum <= 0)
		remove();
}

void AfishSchool::setLoc(vector loc) {
	this->loc = loc;
	if (anim)
		anim->setLoc(loc);
}

void AfishSchool::remove() {
	world::currWorld->removeFishSchool(this);
}

