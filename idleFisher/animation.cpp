#include "animation.h"
#include "main.h"
#include "timer.h"
#include "Image.h"
#include "shaderClass.h"
#include "GPULoadCollector.h"

#include <functional>
#include <iostream>

#include "debugger.h"

animation::animation(std::string spriteSheetPath, int cellWidth, int cellHeight, std::unordered_map<std::string, animDataStruct> animData, bool useWorldLoc, vector loc) {
	this->cellSize = { static_cast<float>(cellWidth), static_cast<float>(cellHeight) };
	this->animData = animData;

	spriteSheet = std::make_shared<Image>("images/" + spriteSheetPath, loc, useWorldLoc);
	cellNum.x = round(spriteSheet->getSize().x / static_cast<float>(cellWidth));
	cellNum.y = round(spriteSheet->getSize().y / static_cast<float>(cellHeight));

	animTimer = CreateDeferred<Timer>();
	animTimer->addCallback(this, &animation::animCallBack);
}

animation::animation(std::shared_ptr<Image> spriteSheetImg, int cellWidth, int cellHeight, std::unordered_map<std::string, animDataStruct> animData, bool useWorldLoc, vector loc) {
	this->cellSize = { static_cast<float>(cellWidth), static_cast<float>(cellHeight) };
	this->animData = animData;

	std::shared_ptr<Rect> source = std::make_shared<Rect>(0.f, 0.f, spriteSheetImg->getSize().x, spriteSheetImg->getSize().y);
	spriteSheet = std::make_shared<Image>(spriteSheetImg, source, loc, useWorldLoc); // create own instance of image

	cellNum.x = round(spriteSheet->getSize().x / static_cast<float>(cellWidth));
	cellNum.y = round(spriteSheet->getSize().y / static_cast<float>(cellHeight));

	animTimer = CreateDeferred<Timer>();
	animTimer->addCallback(this, &animation::animCallBack);
}

animation::~animation() {
	stop();
	eventCallback_ = nullptr;
	finishedCallback_ = nullptr;
	frameCallback_ = nullptr;
}

void animation::draw(Shader* shaderProgram) {
	spriteSheet->draw(shaderProgram);
}

void animation::start() {
	// reset frame back to beginning
	currFrameLoc = animData[currAnim].start;
	bStopped = false;
	bFinished = false;
	if (animTimer)
		animTimer->start(animData[currAnim].duration == 0 ? stuff::animSpeed : animData[currAnim].duration, true);

	// set source
	spriteSheet->setSourceRect(std::make_shared<Rect>(currFrameLoc.x * cellSize.x, currFrameLoc.y * cellSize.y, cellSize.x, cellSize.y));

	if (frameCallback_)
		frameCallback_(calcFrameDistance(true));
}

void animation::stop() {
	currFrameLoc = animData[currAnim].start;
	bStopped = true;
	bFinished = false;
	if (animTimer)
		animTimer->stop();
}

void animation::setAnimation(const std::string& name) {
	auto it = animData.find(name);
	if (it == animData.end()) {
		std::cout << "Invalid Animation: \"" << name << "\"\n";
		abort();
	}

	animTimer->SetTime(it->second.duration == 0 ? stuff::animSpeed : it->second.duration);

	int frameNum = calcFrameDistance(true);

	currAnim = name;

	// if prev animation was on a greater frame than current anim then reset frameNum
	if (frameNum > calcFrameDistance(false))
		frameNum = 0;

	currFrameLoc = animData[currAnim].start + vector{ float(frameNum), 0 };

	if (currFrameLoc.x > cellNum.x - 1)
		currFrameLoc = animData[currAnim].start;

	spriteSheet->setSourceRect(std::make_shared<Rect>(currFrameLoc.x * cellSize.x, currFrameLoc.y * cellSize.y, cellSize.x, cellSize.y));

	// updates loc, so it isn't offset by the change in source
	setLoc(getLoc());
}

void animation::animCallBack() {
	if (bStopped)
		return;

	currFrameLoc.x++;

	// if out side of img rect || if past the end loc
	if (currFrameLoc.x > animData[currAnim].end.x && currFrameLoc.y == animData[currAnim].end.y) {
		currFrameLoc = animData[currAnim].start;
		if (!animData[currAnim].loop) {
			if (animTimer)
				animTimer->stop();
			bFinished = true;
		}
		if (finishedCallback_) {
			finishedCallback_();
			if (bStopped)
				return;
		}
	}

	if (frameCallback_) {
		//std::cout << "start: " << animData[currAnim].start << ", end: " << animData[currAnim].end << ", frame: " << currFrameLoc << std::endl;
		frameCallback_(calcFrameDistance(true));
		//frameCallback_(frameNum % animList.size());
	}

	if (eventCallback_ && calcFrameDistance(true) == eventFrameNum - 1)
		eventCallback_();

	spriteSheet->setSourceRect(std::make_shared<Rect>(currFrameLoc.x * cellSize.x, currFrameLoc.y * cellSize.y, cellSize.x, cellSize.y));
}

void animation::addFinishedCallback(void (*callback) ()) {
	finishedCallback_ = callback;
}

bool animation::IsFinished() {
	return bFinished;
}

bool animation::IsStopped() {
	return bStopped;
}

int animation::calcFrameDistance(bool getFrameNum) {
	vector start = animData[currAnim].start;
	vector end;
	if (getFrameNum)
		end = currFrameLoc;
	else
		end = animData[currAnim].end;

	return end.x - start.x;
}

void animation::setLoc(vector loc) {
	spriteSheet->setLoc(loc);
}

vector animation::getLoc() {
	return spriteSheet->getLoc();
}

vector animation::getAbsoluteLoc() {
	return spriteSheet->getAbsoluteLoc();
}

void animation::SetAnchor(Anchor xAnchor, Anchor yAnchor) {
	spriteSheet->SetAnchor(xAnchor, yAnchor);
}

void animation::SetPivot(vector pivot) {
	spriteSheet->SetPivot(pivot);
}

vector animation::GetCellSize() {
	return cellSize;
}

vector animation::GetCellNum() {
	return cellNum;
}

std::string animation::GetCurrAnim() {
	return currAnim;
}

void animation::SetColorMod(glm::vec4 colorMod) {
	if (spriteSheet)
		spriteSheet->setColorMod(colorMod);
}

bool animation::IsMouseOver(bool useAlpha) {
	if (!spriteSheet)
		return false;
	return spriteSheet->isMouseOver(useAlpha);
}

void animation::SetUseAlpha(bool useAlpha) {
	if (spriteSheet)
		spriteSheet->setUseAlpha(useAlpha);
}

void animation::SetAnimDuration(std::string animName, float duration) {
	animData[animName].duration = duration;
}

void animation::SetCurrAnimDuration(float duration) {
	animData[currAnim].duration = duration;
	animTimer->SetTime(duration);
}

float animation::GetAnimDuration(std::string animName) {
	return animData[animName].duration;
}

float animation::GetCurrAnimDuration() {
	return animData[currAnim].duration;
}

void animation::SetCurrFrameLoc(vector loc) {
	if (loc.x != -1)
		currFrameLoc.x = math::clamp(loc.x, 0, cellNum.x);
	if (loc.y != -1)
		currFrameLoc.y = math::clamp(loc.y, 0, cellNum.y);

	// update the source rect
	spriteSheet->setSourceRect(std::make_shared<Rect>(currFrameLoc.x * cellSize.x, currFrameLoc.y * cellSize.y, cellSize.x, cellSize.y));
}

vector animation::GetCurrFrameLoc() {
	return currFrameLoc;
}