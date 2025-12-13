#include "fishUnlocked.h"

#include "main.h"
#include "saveData.h"
#include "timer.h"
#include "widget.h"
#include "animation.h"
#include "text.h"

#include "debugger.h"

UfishUnlocked::UfishUnlocked(widget* parent) : widget(parent) {
	bannerImg = std::make_unique<Image>("./images/widget/fishUnlocked.png", vector{ 0, 0 }, false);

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "normal", animDataStruct({0, 0}, {4, 0}, false) });
	animData.insert({ "reverse", animDataStruct({0, 1}, {4, 1}, false) });
	anim = std::make_unique<animation>("widget/fishUnlockedSpritesheet.png", 135, 63, animData, false, vector{ 0, 0 }); // 116*1.5, 67*1.5
	anim->setAnimation("normal");
	anim->addFinishedCallback(this, &UfishUnlocked::finished);

	finishedTimer = std::make_unique<timer>();
	finishedTimer->addCallback(this, &UfishUnlocked::reverse);

	visible = false;
}

UfishUnlocked::~UfishUnlocked() {

}

void UfishUnlocked::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	if (anim->finished()) {
		bannerImg->draw(shaderProgram);
		thumbnail->draw(shaderProgram);
		if (name)
			name->draw(shaderProgram);
	} else
		anim->draw(shaderProgram);
}

void UfishUnlocked::start(FfishData fish) {
	visible = true;
	thumbnail = std::make_unique<Image>(fish.thumbnail, vector{ 0, 0 }, false);
	finishedTimer->start(4);
	setupLocs();

	anim->setAnimation("normal");
	anim->start();
}

void UfishUnlocked::start(FachievementStruct* achievementData) {
	visible = true;
	thumbnail = std::make_unique<Image>("./images/widget/achievementIcons/achievementIcon" + std::to_string(achievementData->id + 1) + ".png", vector{ 0, 0 }, false);
	name = std::make_unique<text>(this, achievementData->name, "straight", vector{ 0, 0 });
	name->setLineLength(50 * stuff::pixelSize);

	finishedTimer->start(4);
	setupLocs();

	anim->setAnimation("normal");
	anim->start();
}

void UfishUnlocked::setupLocs() {
	__super::setupLocs();

	vector center = { stuff::screenSize.x / 2.f - bannerImg->w / 2 * stuff::pixelSize, 17 * stuff::pixelSize};
	bannerImg->setLoc(center);
	//anim->setLoc(center - vector{ 20, 26 } * stuff::pixelSize);
	anim->setLoc(bannerImg->getLoc());

	if (thumbnail)
		thumbnail->setLoc(bannerImg->getLoc() + vector{30, 10} * stuff::pixelSize);// center + vector{ 17 + 20, 3 + 26 } *stuff::pixelSize;
	if (name)
		name->setLoc(bannerImg->getLoc() + vector{58, 14} *stuff::pixelSize);//center + vector{ 42, 5 } * stuff::pixelSize);
}

void UfishUnlocked::reverse() {
	anim->setAnimation("reverse");
	anim->start();
}

void UfishUnlocked::finished() {
	if (anim->currAnim == "reverse")
		visible = false;
}