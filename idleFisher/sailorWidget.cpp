#include "sailorWidget.h"
#include "main.h"
#include "saveData.h"
#include "shortNumbers.h"
#include "currencyWidget.h"
#include "button.h"
#include "text.h"
#include "map.h"
#include "animation.h"
#include "timer.h"

#include "debugger.h"

UsailorWidget::UsailorWidget(widget* parent) : widget(parent) {
	mapBackground = std::make_unique<Image>("./images/widget/map.png", vector{ 0, 0 }, false);

	setupLocs(); // setup so map can get loc of map background

	vector mapSize = mapBackground->getSize() - vector{ 70, 70 } * stuff::pixelSize;
	map = std::make_unique<Umap>(this, mapSize);

	xButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	xButton->addCallback(this, &UsailorWidget::closeWidget);

	mapClosed = std::make_unique<Image>("./images/widget/mapClosed.png", vector{ 0, 0 }, false);

	std::unordered_map < std::string, animDataStruct> anim;
	anim.insert({ "open", animDataStruct({0, 0}, {5, 0}, false) });
	anim.insert({ "close", animDataStruct({0, 1}, {5, 1}, false) });
	mapAnim = std::make_unique<animation>("widget/mapSpriteSheet.png", 402, 230, anim, false, vector{ 0, 0 });
	mapAnim->addFinishedCallback(this, &UsailorWidget::mapAnimFinish);
	mapAnim->setAnimation("open");

	mapTimer = std::make_unique<timer>();
	mapTimer->addUpdateCallback(this, &UsailorWidget::moveAnim);
	mapTimer->addCallback(this, &UsailorWidget::mapTimerFinish);

	setupLocs();
}

UsailorWidget::~UsailorWidget() {

}

void UsailorWidget::draw(Shader* shaderProgram) {
	if (mapClosed && !mapTimer->IsFinished())
		mapClosed->draw(shaderProgram);
	if (mapAnim && !mapAnim->IsFinished() && mapTimer->IsFinished())
		mapAnim->draw(shaderProgram);

	if (!mapAnim || !mapAnim->IsFinished() || !mapTimer->IsFinished())
		return;

	__super::draw(shaderProgram);

	mapBackground->draw(shaderProgram);

	// draw only inside specified area
	vector mapSize = mapBackground->getSize() - vector{ 70, 70 } * stuff::pixelSize;

	map->draw(shaderProgram);

	if (xButton) {
		xButton->draw(shaderProgram);
	}
}

void UsailorWidget::setupLocs() {
	if (mapBackground)
		mapBackground->setLoc(vector{ stuff::screenSize.x / 2.f, stuff::screenSize.y / 2.f } - mapBackground->getSize() / 2.f);
	if (xButton)
		xButton->setLoc(mapBackground->getLoc() + vector{mapBackground->getSize().x - 13.5f * stuff::pixelSize, 5 * stuff::pixelSize});

	if (mapAnim && mapBackground)
		mapAnim->setLoc(mapBackground->getLoc());
}

void UsailorWidget::addedToViewport() {
	if (opening || closing)
		return;

	if (open) {
		closeWidget();
		return;
	}

	opening = true;
	mapClosed->setLoc(stuff::screenSize);
	mapTimer->start(.5);
}

void UsailorWidget::moveAnim() {
	float percent = mapTimer->getTime() / mapTimer->getMaxTime();

	std::vector<vector> locs = { {0, 1000}, { 0, 500 }, {0, 100}, {0, 30}, {0, 10}, {0, 0}, {0, 0} };
	if (opening) // open anim
		mapClosed->setLoc(mapBackground->getLoc() + locs[floor(percent * 6)]);
	else // close anim
		mapClosed->setLoc(mapBackground->getLoc() + locs[floor((1 - percent) * 6)]);
}

void UsailorWidget::mapTimerFinish() {
	if (opening) {
		std::cout << "opening\n";
		mapAnim->setAnimation("open");
		mapAnim->start();
		closing = false;
	} else {
		std::cout << "closing\n";
		removeFromViewport();
		opening = false;
		closing = false;
	}
}

void UsailorWidget::mapAnimFinish() {
	//mapOpenAnim->start();
	opening = false;

	if (mapAnim->GetCurrAnim() == "open")
		open = true;
	else if (mapAnim->GetCurrAnim() == "close")
		mapTimer->start(.5);
}

void UsailorWidget::closeWidget() {
	mapAnim->setAnimation("close");
	mapAnim->start();
	opening = false;
	closing = true;
	open = false;
}

void UsailorWidget::removeFromViewport() {
	closing = false;
	opening = false;
	open = false;
	mapTimer->stop();
	mapAnim->stop();

	__super::removeFromViewport();
}