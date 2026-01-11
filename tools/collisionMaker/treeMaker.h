#pragma once

#include "math.h"

class Atree;

class treeMaker {
public:
	// spawns tree at mouse location
	static void spawnTree(bool isTree);
	// sets tree to move
	static void moveTree();
	static void resetPosition();
	// updates tree loc if selected to move
	static void draw(SDL_Renderer* renderer);
	static void sortWhenAdd(Atree* tree);
	static void sortList();
	// sorts both the bushes and trees combined
	static void combinedSort();

	static void save();
	static void load();

	static inline std::vector<Atree*> trees;
	//static inline std::vector<Atree*> bushes;
	static inline Atree* selectedTree;
	static inline vector selectOffset;
	static inline vector prevLoc;
};
