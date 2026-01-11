#include "tree.h"
#include "main.h"

Atree::Atree(vector loc, bool isTree) {
	this->loc = loc;
	this->isTree = isTree;
	treeImg = new Fimg();
	std::string name = isTree ? "tree1" : "bush";
	treeImg->init("./images/landscape/" + name + ".png", loc);
	treeImg->loadSurface("./images/landscape/" + name + ".png");
	vector treeBotMid = vector { treeImg->w * .5f, treeImg->h * 1.f };
	treeImg->loc = loc - treeBotMid;
}

void Atree::setLoc(vector loc) {
	vector treeBotMid = vector{ treeImg->w * .5f, treeImg->h * 1.f };
	this->loc = loc;
	treeImg->loc = loc - treeBotMid;
}

void Atree::draw(SDL_Renderer* renderer) {
	treeImg->draw(renderer);
}

bool Atree::isMouseOver() {
	vector mousePos = Main::mousePos;
		vector min = math::worldToScreen(treeImg->loc);
		vector max = min + (vector{ float(treeImg->w), float(treeImg->h) } * Main::pixelSize);

		if (min.x <= Main::mousePos.x && Main::mousePos.x <= max.x && min.y <= Main::mousePos.y && Main::mousePos.y <= max.y) {
			vector screenPos = min;
			vector pos = { Main::mousePos.x - screenPos.x, Main::mousePos.y - screenPos.y };
			SDL_Color pixelColor = math::GetPixelColor(treeImg->surface, (int)pos.x, (int)pos.y);

			if ((int)pixelColor.a != 0)
				return true;
		}
	return false;
}