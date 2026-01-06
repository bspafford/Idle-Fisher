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
	mapBackground = std::make_unique<Image>("images/widget/map.png", vector{ 0, 0 }, false);
	mapBackground->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	mapBackground->SetPivot({ 0.5f, 0.5f });

	setupLocs(); // setup so map can get loc of map background

	vector mapSize = mapBackground->getSize() - vector{ 70.f, 60.f };
	map = std::make_unique<Umap>(this, mapSize);

	xButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	xButton->SetPivot({ 0.5f, 0.5f });
	xButton->addCallback(this, &UsailorWidget::closeWidget);

	mapClosed = std::make_unique<Image>("images/widget/mapClosed.png", vector{ 0, 0 }, false);
	mapClosed->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	mapClosed->SetPivot({ 0.5f, 0.5f });

	std::unordered_map < std::string, animDataStruct> anim;
	anim.insert({ "open", animDataStruct({0, 0}, {5, 0}, false) });
	anim.insert({ "close", animDataStruct({0, 1}, {5, 1}, false) });
	mapAnim = std::make_unique<animation>("widget/mapSpriteSheet.png", 402, 230, anim, false, vector{ 0, 0 });
	mapAnim->addFinishedCallback(this, &UsailorWidget::mapAnimFinish);
	mapAnim->setAnimation("open");

	mapTimer = CreateDeferred<Timer>();
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

	map->draw(shaderProgram);

	if (xButton) {
		xButton->draw(shaderProgram);
	}
}

void UsailorWidget::setupLocs() {
	if (xButton)
		xButton->setLoc(mapBackground->getAbsoluteLoc() + mapBackground->getSize() - 10.f);

	if (mapAnim && mapBackground)
		mapAnim->setLoc(mapBackground->getAbsoluteLoc());
}

void UsailorWidget::addedToViewport() {
	if (opening || closing)
		return;

	if (open) {
		closeWidget();
		return;
	}

	opening = true;
	mapTimer->start(.5);

	map->SetCurrWorldToCenter();
}

void UsailorWidget::moveAnim() {
	float percent = mapTimer->getTime() / mapTimer->getMaxTime();

	std::vector<float> percents = { -0.926f, -0.463f, -0.093f, -0.028f, -0.009f, 0.f, 0.f };
	if (opening) // open anim
		mapClosed->setLoc(vector{ 0.f, percents[floor(percent * 6)] * stuff::screenSize.y / stuff::pixelSize });
	else // close anim
		mapClosed->setLoc(vector{ 0.f, percents[floor((1 - percent) * 6)] * stuff::screenSize.y / stuff::pixelSize });
}

void UsailorWidget::mapTimerFinish() {
	if (opening) {
		mapAnim->setAnimation("open");
		mapAnim->start();
		closing = false;
	} else {
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