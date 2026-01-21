#include "worlds.h"

#include "main.h"

// world 1
world1::world1() {
	mapImg.init("../../idleFisher/images/worlds/demo/collisionHelper.png", { 0, 0 });
	mapImg.loc.y = -mapImg.h;
	inFront.init("./images/dockInfront.png", { 0, 0 });
}

void world1::start() {
	// on init make the circle appear
}

void world1::draw(SDL_Renderer* renderer) {
	// draw background
	mapImg.draw(renderer);
}