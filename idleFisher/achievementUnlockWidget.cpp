#include "achievementUnlockWidget.h"

#include "main.h"
#include "saveData.h"
#include "timer.h"
#include "widget.h"
#include "animation.h"
#include "text.h"

#include "debugger.h"

UachievementUnlockWidget::UachievementUnlockWidget(widget* parent) : widget(parent) {
	bannerImg = std::make_unique<Image>("images/widget/fishUnlocked.png", vector(0, 0), false);
	bannerImg->SetAnchor(ANCHOR_CENTER, ANCHOR_TOP);
	bannerImg->SetPivot(vector(0.5f, 1.f));

	thumbnail = std::make_unique<Image>("", vector(0, 0), false);
	thumbnail->SetPivot(vector(0.f, 0.5f));
	name = std::make_unique<text>(this, "---", "straight", vector(0, 0));
	name->setLineLength(50);

	std::unordered_map<std::string, animDataStruct> animData;
	animData.insert({ "normal", animDataStruct({0, 0}, {4, 0}, false) });
	animData.insert({ "reverse", animDataStruct({0, 1}, {4, 1}, false) });
	anim = std::make_unique<animation>("widget/fishUnlockedSpritesheet.png", 135, 63, animData, false, vector(0, 0));
	anim->setAnimation("normal");
	anim->addFinishedCallback(this, &UachievementUnlockWidget::finished);

	finishedTimer = CreateDeferred<Timer>();
	finishedTimer->addCallback(this, &UachievementUnlockWidget::reverse);

	visible = false;

	unlockAudio = std::make_unique<Audio>("achievement.wav", AudioType::SFX);
}

UachievementUnlockWidget::~UachievementUnlockWidget() {

}

void UachievementUnlockWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	if (anim->IsFinished()) {
		bannerImg->draw(shaderProgram);
		thumbnail->draw(shaderProgram);
		if (name)
			name->draw(shaderProgram);
	} else
		anim->draw(shaderProgram);
}

void UachievementUnlockWidget::start(const FachievementStruct& achievementData) {
	setVisibility(true);

	thumbnail->setImage("images/widget/achievementIcons/achievementIcon" + std::to_string(achievementData.id) + ".png");
	name->setText(achievementData.name);

	finishedTimer->stop();
	finishedTimer->start(4);
	setupLocs();

	anim->setAnimation("normal");
	anim->stop();
	anim->start();

	unlockAudio->Play();
}

void UachievementUnlockWidget::setupLocs() {
	__super::setupLocs();

	bannerImg->setLoc(vector(0.f, -17.f));
	anim->setLoc(bannerImg->getAbsoluteLoc() + vector(0.f, -14.f));

	float centerY = bannerImg->getSize().y / 2.f + 3.f;
	if (thumbnail)
		thumbnail->setLoc(bannerImg->getAbsoluteLoc() + vector(25, centerY));
	if (name)
		name->setLoc(bannerImg->getAbsoluteLoc() + vector(54, centerY));
}

void UachievementUnlockWidget::reverse() {
	anim->setAnimation("reverse");
	anim->start();
}

void UachievementUnlockWidget::finished() {
	if (anim->GetCurrAnim() == "reverse")
		visible = false;
}