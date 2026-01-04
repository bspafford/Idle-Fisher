#include "background.h"
#include "Rectangle.h"

Background::Background(widget* parent, std::string path, glm::vec4 fillColor) : widget(parent) {
	this->path = path;

	// load images
	std::string edgeNames[] = { "Top", "Right", "Bottom", "Left" };
	std::string cornerNames[] = { "TopLeft", "TopRight", "BottomRight", "BottomLeft" };
	for (int i = 0; i < 4; i++) {
		edges.push_back(std::make_unique<Image>("./images/" + path + "/edge" + edgeNames[i] + ".png", vector{0.f, 0.f}, false));
		corners.push_back(std::make_unique<Image>("./images/" + path + "/corner" + cornerNames[i] + ".png", vector{0.f, 0.f}, false));
	}

	fillRect = std::make_unique<URectangle>(this, vector{ 0.f, 0.f }, vector{ 0.f, 0.f }, false, fillColor);
}

void Background::draw(Shader* shaderProgram) {
	fillRect->draw(shaderProgram);
	for (int i = 0; i < 4; i++) {
		edges[i]->draw(shaderProgram);
		corners[i]->draw(shaderProgram);
	}
}

void Background::setLoc(vector loc) {
	widget::setLoc(loc);

	corners[0]->setLoc(absoluteLoc + vector{ 0.f, size.y - corners[0]->getSize().y });	// top left
	corners[1]->setLoc(absoluteLoc + size - corners[1]->getSize());						// top right
	corners[2]->setLoc(absoluteLoc + vector{ size.x - corners[2]->getSize().x, 0.f });	// bottom right
	corners[3]->setLoc(absoluteLoc);													// bottom left

	edges[0]->setLoc(absoluteLoc + vector{ corners[0]->getSize().x, size.y - edges[0]->getSize().y});	// top
	edges[1]->setLoc(absoluteLoc + vector{ size.x - edges[1]->getSize().x, corners[2]->getSize().y });	// right
	edges[2]->setLoc(absoluteLoc + vector{ corners[3]->getSize().x, 0.f });								// bottom
	edges[3]->setLoc(absoluteLoc + vector{ 0.f, corners[3]->getSize().y });								// left

	edges[0]->setSize({ corners[1]->getAbsoluteLoc().x - (corners[0]->getAbsoluteLoc().x + corners[0]->getSize().x), edges[0]->getSize().y });	// top
	edges[1]->setSize({ edges[1]->getSize().x, corners[1]->getAbsoluteLoc().y - (corners[2]->getAbsoluteLoc().y + corners[2]->getSize().y) });	// right
	edges[2]->setSize({ corners[2]->getAbsoluteLoc().x - (corners[3]->getAbsoluteLoc().x + corners[3]->getSize().x), edges[2]->getSize().y });	// bottom
	edges[3]->setSize({ edges[3]->getSize().x, corners[0]->getAbsoluteLoc().y - (corners[3]->getAbsoluteLoc().y + corners[3]->getSize().y) });	// left

	vector min = absoluteLoc + vector{ edges[3]->getSize().x, edges[2]->getSize().y };
	vector max = vector{ edges[1]->getAbsoluteLoc().x, edges[0]->getAbsoluteLoc().y };
	fillRect->setLoc(min);
	fillRect->setSize(max - min);
}

void Background::setSize(vector size) {
	widget::setSize(size);
	setLoc(loc);
}