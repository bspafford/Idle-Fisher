#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include <vector>
#include <iostream>

struct vector {
	float x = 0;
	float y = 0;
};

// image stuff
struct Fimg {
	std::string path;
	SDL_Texture* texture;
	SDL_Surface* surface;
	int w, h;
	vector loc;

	void init(std::string path, vector loc = { 0, 0 });
	void loadSurface(std::string path);
	void draw(SDL_Renderer* renderer, bool useCharLoc = true);
};

// operator overloading
inline vector operator+ (vector a, vector b) {
	return { a.x + b.x, a.y + b.y };
}

inline vector operator- (vector a, vector b) {
	return { a.x - b.x, a.y - b.y };
}

inline vector operator* (vector a, float b) {
	return { a.x * b, a.y * b };
}

inline vector operator*= (vector a, float b) {
	return { a.x * b, a.y * b };
}

inline std::ostream& operator<< (std::ostream& os, vector& a) {
	os << a.x << ", " << a.y;
	return os;
}

class math {
public:
	static float dot(vector a, vector b);
	static vector normalize(vector a);
	static float length(vector value);
	static float distance(float x1, float y1, float x2, float y2);
	static float min(float a, float b);
	static float max(float a, float b);
	static float lerp(float min, float max, float val);
	static float randRange(float min, float max);

	// helper functions
	static vector screenToWorld(float x, float y);
	static vector screenToWorld(vector pos);

	static vector worldToScreen(float x, float y);
	static vector worldToScreen(vector pos);

	static void pointsToWorld(SDL_Point*& points);

	// sdl stuff
	static SDL_Color GetPixelColor(SDL_Surface* pSurface, const int X, const int Y);
};