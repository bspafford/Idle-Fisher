#include "merchantShop.h"

AmerchantShop::AmerchantShop(vector loc) : Abuilding(loc) {
	buildingImg = std::make_unique<Image>("images/merchantShop.png", loc, true);

	setupCollision({ loc + vector{0, 19}, loc + vector{36, 0}, loc + vector{106, 35}, loc + vector{70, 54} });
}

AmerchantShop::~AmerchantShop() {

}

void AmerchantShop::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);
}