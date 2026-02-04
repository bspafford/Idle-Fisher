#pragma once

// #include "timer.h"

#include <string>
#include <vector>
#include <cassert>
#include <functional>

#include "Anchor.h"
#include "math.h"
#include "deferredPtr.h"

class Timer;
class Image;
class Shader;

struct animDataStruct {
	vector start; // x and y where frame starts (0, 0) is top left
	vector end; // x and y where frame ends
	bool loop; // if the animation loops
	float duration; // how fast the animation is

	animDataStruct() {}
	animDataStruct(vector _start, vector _end, bool _loop, float _duration = 0.f) : start(_start), end(_end), loop(_loop), duration(_duration) {}
};

class animation {
public:
	// if usecharLoc, then object is in world space
	// sprite sheet path already inside image folder, no need to use "images/" anymore
	// animation sheet only works with animations being on one row at a time, cant have half an animation on the next row down
	animation(std::string spriteSheetPath, int cellWidth, int cellHeight, std::unordered_map<std::string, animDataStruct> animData, bool useWorldLoc = true, vector loc = { 0, 0 });
	animation(std::shared_ptr<Image> spriteSheetImg, int cellWidth, int cellHeight, std::unordered_map<std::string, animDataStruct> animData, bool useWorldLoc = true, vector loc = { 0, 0 });
	// used for a lot of the same objects that use the same sprite sheet, for example: autoFisher, trees, bushes, leaves, etc.
	//animation(img* spriteSheetImg, int cellWidth, int cellHeight, std::unordered_map<std::string, animDataStruct> animData, bool useWorldLoc = true, vector loc = { 0, 0 });
	~animation();

	void draw(Shader* shaderProgram);

	void setLoc(vector loc);
	vector getLoc();
	vector getAbsoluteLoc();
	void SetAnchor(Anchor xAnchor, Anchor yAnchor);
	void SetPivot(vector pivot);

	void start();
	void stop();

	void setAnimation(std::string name);

	void animCallBack();

	template <class T> void addAnimEvent(int frameNum, T* const object, void (T::* const callback) ()) {
		eventFrameNum = frameNum;
		eventCallback_ = std::bind_front(callback, object);
	}

	// not static
	template <class T> void addFinishedCallback(T* const object, void (T::* const callback) ()) {
		finishedCallback_ = std::bind_front(callback, object);
	}
	// static
	void addFinishedCallback(void (*callback) ());

	// calls function every animation frame
	template <class T> void addFrameCallback(T* const object, void (T::* const callback) (int)) {
		frameCallback_ = std::bind(callback, object, std::placeholders::_1);
	}

	bool IsFinished();
	bool IsStopped();

	vector GetCellSize();
	vector GetCellNum();

	std::string GetCurrAnim();

	bool IsMouseOver(bool useAlpha);

	void SetColorMod(glm::vec4 colorMod);
	void SetUseAlpha(bool useAlpha);

	void SetAnimDuration(std::string animName, float duration);
	void SetCurrAnimDuration(float duration);
	float GetAnimDuration(std::string animName);
	float GetCurrAnimDuration();

	// can put -1 to keep current value on that axis
	void SetCurrFrameLoc(vector loc);
	vector GetCurrFrameLoc();

	// calculates what frame the animation is on
	// if getFrameNum = true, then function finds distance from start to frame
	// if false, then the function finds distance from start to end
	int calcFrameDistance(bool getFrameNum);

private:
	vector cellSize;
	vector cellNum;

	int eventFrameNum;
	std::function<void()> eventCallback_ = NULL;
	std::function<void()> finishedCallback_ = NULL;
	std::function<void(int)> frameCallback_ = NULL;

	bool bFinished = false;
	bool bStopped = true;

	DeferredPtr<Timer> animTimer;

	std::unordered_map<std::string, animDataStruct> animData;
	std::shared_ptr<Image> spriteSheet;

	std::string currAnim; // which animation is currently selected
	vector currFrameLoc; // which frame the animation is on
};