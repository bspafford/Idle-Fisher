#pragma once

#include <SDL.h>
#include <string>

#include "math.h"

class world1 {
public:
	world1();

	static void start();
	static void draw(SDL_Renderer* renderer);

	static inline Fimg mapImg;
	static inline Fimg inFront;
};