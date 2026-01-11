#include "main.h"

#include <chrono>
#include <iostream>

#include "worlds.h"

#include "collision.h"

#include "treeMaker.h"

int main(int argc, char* argv[]) {
	Main* _main = new Main();
	_main->createWindow();

	return 0;
}

void Main::createWindow() {
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Idle Fisher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // -1 auto selects driver, SDL_RENDERER_ACCELERATED makes it use hardware acceleration


	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_Surface* icon = IMG_Load("./images/icon.png");
	SDL_SetWindowIcon(window, icon);

	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); // set window fullscreen

	running = true;

	// fps timer
	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point currentTime;

	Start();
	draw(renderer);

	SDL_Event e;
	while (running) {
		// fps stuff
		currentTime = std::chrono::steady_clock::now();
		float timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - lastTime).count();
		float deltaTime = timeDiff / 1000000000;
		lastTime = currentTime;

		// run every frame
		Update(deltaTime);

bLeftClick = false;
bRightClick = false;

// user input
while (SDL_PollEvent(&e)) {
	switch (e.type) {
	case SDL_QUIT:
		running = false;
		break;
	case SDL_KEYDOWN:
		if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			running = false;

		// temp
		if (e.key.keysym.scancode == SDL_SCANCODE_K)
			collision::saveCollision(allCollision, collisionIdentifier, collisionWorld);
		if (e.key.keysym.scancode == SDL_SCANCODE_I)
			treeMaker::save();

		if (e.key.keysym.sym == SDLK_LCTRL)
			ctrlDown = true;

		// undo
		if (e.key.keysym.sym == SDLK_z && !KEYS[SDLK_z] && ctrlDown)
			collision::undo();

		if (e.key.keysym.sym <= sizeof(KEYS))
			KEYS[e.key.keysym.sym] = true;

		// spawn tree
		if (e.key.keysym.sym == SDLK_t)
			treeMaker::spawnTree(true);
		if (e.key.keysym.sym == SDLK_b)
			treeMaker::spawnTree(false);

		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button == SDL_BUTTON_LEFT) {
			leftMouseButtonDown = true;
			treeMaker::moveTree();
		}
		if (e.button.button == SDL_BUTTON_RIGHT) {
			treeMaker::resetPosition();
		}

		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button == SDL_BUTTON_LEFT) {
			bLeftClick = true;
			leftMouseButtonDown = false;
			collision::previousActions.push_back(std::vector<vector> { collision::selectedPoint, collision::startLoc});
			collision::selectedPoint.x = -1;
		}
		if (e.button.button == SDL_BUTTON_RIGHT)
			bRightClick = true;

	case SDL_KEYUP:
		if (e.key.keysym.sym <= sizeof(KEYS))
			KEYS[e.key.keysym.sym] = false;
		if (e.key.keysym.sym == SDLK_LCTRL)
			ctrlDown = false;
		break;
	case SDL_MOUSEMOTION:
		mousePos = { (float)e.motion.x, (float)e.motion.y };
		break;
	case SDL_WINDOWEVENT:
		// on window resize
		if (e.window.event == SDL_WINDOWEVENT_RESIZED || e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
			screenWidth = e.window.data1;
			screenHeight = e.window.data2;
		}
		break;
	default:
		break;
	}

}

// draw screen
draw(renderer);
	}

	SDL_Quit();
}

void Main::Start() {
	world1::world1();

	collision::getCollisionObjects(allCollision, collisionIdentifier, collisionWorld);
	treeMaker::load();
}

void Main::Update(float deltaTime) {
	moveDir = { 0, 0 };
	if (KEYS[SDLK_w])
		moveDir = { moveDir.x, -1 };
	if (KEYS[SDLK_s])
		moveDir = { moveDir.x, 1 };
	if (KEYS[SDLK_a])
		moveDir = { -1, moveDir.y };
	if (KEYS[SDLK_d])
		moveDir = { 1, moveDir.y };

	vector normDir = math::normalize(moveDir);
	playerLoc.x += normDir.x * speed * deltaTime;
	playerLoc.y += normDir.y * speed * deltaTime;

	collision::moveCollisionPoint();

	if (bRightClick)
		collision::removeRect();
	if (bLeftClick)
		collision::changeIdentifier();

	collision::mouseOverWater(mousePos, allCollision, collisionIdentifier, collisionWorld);

	if (mousePos.x >= addButtonRect->x && mousePos.x <= addButtonRect->x + addButtonRect->w && mousePos.y >= addButtonRect->y && mousePos.y <= addButtonRect->y + addButtonRect->h) {
		mouseOverButton = true;
		if (bLeftClick)
			collision::addCollisionBox();
	} else
		mouseOverButton = false;
}

void Main::draw(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 104, 153, 185, 255); // background color // 92, 127, 153, 255
	SDL_RenderClear(renderer); // clears screen

	world1::draw(renderer);

	treeMaker::draw(renderer);

	collision::showCollisionBoxes(renderer, allCollision, collisionIdentifier, collisionWorld);

	// draw collision
	// collision::showCollisionBoxes(renderer, allCollision, collisionIdentifier, collisionWorld);

	// always draw last!!! // box that follows mouse
	// const SDL_Rect cursorRect{ mousePos.x, mousePos.y, cursorW * stuff::pixelSize, cursorH * stuff::pixelSize };
	// SDL_RenderCopy(renderer, cursorImg, NULL, &cursorRect);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, addButtonRect);

	// finally return that points thats the closest to the rotation

	SDL_RenderPresent(renderer); // draws the renderer to the screen
}