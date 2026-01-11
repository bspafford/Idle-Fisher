#include "worlds.h"

#include "main.h"

// world 1
world1::world1() {
	mapImg.init("Q:/Documents/VisualStudios/SDL2/images/worlds/world1/map1.png", { -500, -775 });
	inFront.init("./images/dockInfront.png", { -500, -775 });
}

void world1::start() {
	// on init make the circle appear
}

void world1::draw(SDL_Renderer* renderer) {
	// draw background
	mapImg.draw(renderer);
}