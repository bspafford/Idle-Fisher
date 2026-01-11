#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "math.h"

class Main {
public:
	void createWindow();
	void Start();
	void Update(float deltaTime);

	void draw(SDL_Renderer* renderer);

	static inline SDL_Renderer* renderer;

	static inline int screenWidth = 1920;
	static inline int screenHeight = 1080;

	bool running = false;

	static inline vector mousePos;

	static inline bool bLeftClick = false, bRightClick = false;

	bool KEYS[322];

	static inline vector playerLoc = { -1131.290039, -440.191193 };
	static inline float pixelSize = 4;

	static inline vector moveDir;
	float speed = 1000;

	inline static std::vector<SDL_Point*> allCollision;
	inline static std::vector<std::string> collisionIdentifier;
	inline static std::vector<std::string> collisionWorld;

	static inline std::string currWorld = "world1";

	static inline bool leftMouseButtonDown = false;

	static inline bool ctrlDown = false;

	SDL_Rect* addButtonRect = new SDL_Rect{ 10, 10, 150, 75 };

	static inline bool mouseOverButton = false;
};