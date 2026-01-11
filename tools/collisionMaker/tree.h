#pragma once

#include "math.h"

class Atree {
public:
	Atree(vector loc, bool isTree);
	void draw(SDL_Renderer* renderer);
	bool isMouseOver();
	void setLoc(vector loc);
	vector loc;
	Fimg* treeImg;
	bool isTree;
};