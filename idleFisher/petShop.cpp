#include "petShop.h"

ApetShop::ApetShop(vector loc) : Abuilding(loc) {
	buildingImg = std::make_unique<Image>("images/petShop.png", loc, true);
}

ApetShop::~ApetShop() {

}

void ApetShop::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);
}