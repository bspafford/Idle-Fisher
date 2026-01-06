#include "mechanicHouse.h"

AmechanicHouse::AmechanicHouse(vector loc) : Abuilding(loc) {
	buildingImg = std::make_unique<Image>("images/mechanicHouse.png", loc, true);

	setupCollision({ loc + vector{2, 42}, loc + vector{69, 8}, loc + vector{133, 40}, loc + vector{66, 42+32} });
}

AmechanicHouse::~AmechanicHouse() {

}

void AmechanicHouse::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);
}