#include "math.h"

#include <iostream>

#include "main.h"

// setup stuff to draw image
void Fimg::init(std::string path1, vector loc) {
	path = path1;
	texture = IMG_LoadTexture(Main::renderer, path.c_str());
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	this->loc = loc;
}

void Fimg::loadSurface(std::string path) {
	surface = IMG_Load(path.c_str());
}

// draw image
void Fimg::draw(SDL_Renderer* renderer, bool useCharLoc) {

	SDL_Rect* rect;
	if (useCharLoc) {
		vector worldLoc = math::worldToScreen(loc);
		rect = new SDL_Rect{ int(worldLoc.x), int(worldLoc.y), int(w * Main::pixelSize), int(h * Main::pixelSize) };
	}
	else
		rect = new SDL_Rect{ int(loc.x), int(loc.y), int(w * Main::pixelSize), int(h * Main::pixelSize) };
	SDL_RenderCopy(renderer, texture, NULL, rect);
}

// returns the dot product of two 2d vectors
float math::dot(vector a, vector b) {
	return a.x * b.x + a.y * b.y;
}

// normalizes the vector
vector math::normalize(vector a) {
	float invLen = 1 / sqrtf(a.x * a.x + a.y * a.y);

	if (a.x == 0 && a.y == 0)
		return { 0, 0 };

	return { a.x * invLen, a.y * invLen };

}

float math::length(vector value) {
	return sqrtf(value.x * value.x + value.y * value.y);
}


// returns the distance between 2 points
float math::distance(float x1, float y1, float x2, float y2) {
	return sqrtf(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

}

float math::min(float a, float b) {
	if (a < b) return a;
	return b;
}

float math::max(float a, float b) {
	if (a > b) return a;
	return b;
}

float math::lerp(float min, float max, float val) {
	float dif = max - min;
	return val * dif + min;
}

float math::randRange(float min, float max) {
	return (float)rand() / RAND_MAX * (max - min) + min;
}

// converts screen coords to world coords
vector math::screenToWorld(float x, float y) {
	vector temp = { floor((Main::playerLoc.x + x) / Main::pixelSize), floor((Main::playerLoc.y + y) / Main::pixelSize) };
	return temp;
}
vector math::screenToWorld(vector pos) {
	vector temp = { floor((Main::playerLoc.x + pos.x) / Main::pixelSize), floor((Main::playerLoc.y + pos.y) / Main::pixelSize) };
	return temp;
}

// converts world coords to screen coords
vector math::worldToScreen(float x, float y) {
	float screenX = x * Main::pixelSize - Main::playerLoc.x;
	float screenY = y * Main::pixelSize - Main::playerLoc.y;

	return { screenX, screenY };
}
vector math::worldToScreen(vector pos) {
	float screenX = pos.x * Main::pixelSize - Main::playerLoc.x;
	float screenY = pos.y * Main::pixelSize - Main::playerLoc.y;

	return { screenX, screenY };
}

void math::pointsToWorld(SDL_Point*& points) {

	SDL_Point* newPoints = new SDL_Point[5];

	for (int i = 0; i < 5; i++) {
		vector point = worldToScreen(points[i].x, points[i].y);
		newPoints[i] = { (int)point.x, (int)point.y };
	}

	points = newPoints;
}

SDL_Color math::GetPixelColor(SDL_Surface* pSurface, const int X, const int Y) {
	const int x = X / Main::pixelSize;
	const int y = Y / Main::pixelSize;

	int bpp = pSurface->format->BytesPerPixel;
	Uint8* p = (Uint8*)pSurface->pixels + y * pSurface->pitch + x * bpp;
	SDL_Color Color = { 0, 0, 0, 0 };

	// checks if within image
	if (bpp == 4 && x <= pSurface->w && y <= pSurface->h) {
		Uint32 PixelColor = p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
		SDL_GetRGBA(PixelColor, pSurface->format, &Color.r, &Color.g, &Color.b, &Color.a);
		return Color;
	}

	return Color;
}