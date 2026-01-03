#include "achievementBox.h"

#include "Input.h"
#include "saveData.h"
#include "timer.h"

#include "debugger.h"

UachievementBox::UachievementBox(widget* parent, int id) : widget(parent) {
	this->id = id;

	if (!rotateTimer) {
		rotateTimer = CreateDeferred<Timer>();
		rotateTimerSpeed = 4;
		rotateTimer->addUpdateCallback(UachievementBox::rotateUpdate);
		rotateTimer->addCallback(UachievementBox::finishRotateTimer);
		rotateTimer->start(rotateTimerSpeed);
	}

	achievementStruct = &SaveData::data.achievementData[id];
	saveAchievementStruct = &SaveData::saveData.achievementList[id];

	if (saveAchievementStruct->unlocked) {
		icon = std::make_unique<Image>("./images/widget/achievementIcons/achievementIcon" + std::to_string(id + 1) + ".png", vector{ 0, 0 }, false);
	} else
		icon = std::make_unique<Image>("./images/widget/achievementIcons/locked.png", vector{ 0, 0 }, false);

	startSize = icon->getSize();
}

UachievementBox::~UachievementBox() {

}

void UachievementBox::rotateUpdate() {
	float percent = rotateTimer->getTime() / rotateTimer->getMaxTime();

	rot = sin(percent * 2 * M_PI) * 5;
}

void UachievementBox::finishRotateTimer() {
	rotateTimer->start(rotateTimerSpeed);
}

void UachievementBox::updateAchievementImage() {
	if (saveAchievementStruct->unlocked) {
		icon = std::make_unique<Image>("./images/widget/achievementIcons/achievementIcon" + std::to_string(id + 1) + ".png", vector{ 0, 0 }, false);
	} else
		icon = std::make_unique<Image>("./images/widget/achievementIcons/locked.png", vector{ 0, 0 }, false);
}

void UachievementBox::draw(Shader* shaderProgram) {
	if (icon) {
		if (mouseOver()) {
			float scale = 1.3f;
			icon->setSize(startSize * scale);

			vector test = (icon->getSize() / stuff::pixelSize);
			icon->setLoc(loc - (icon->getSize() - startSize) / 2);

			if (saveAchievementStruct->unlocked)
				icon->setRotation(rot);
		} else {
			icon->setSize(startSize);
			icon->setLoc(loc);
		}
		icon->draw(shaderProgram);
	}
}

vector UachievementBox::getSize() {
	if (icon)
		return startSize;
	return { 0, 0 };
}
bool UachievementBox::mouseOver() {
	return icon->isMouseOver();
}