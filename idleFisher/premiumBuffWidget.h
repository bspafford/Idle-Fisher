#pragma once

#include "widget.h"
#include "saveData.h"

class Timer;
class UprogressBar;
class UhoverBox;
class Image;

class UpremiumBuffWidget : public widget {
public:
	UpremiumBuffWidget(widget* parent, FgoldenFishStruct goldenFish);
	void draw(Shader* shaderProgram);
	void setLoc(vector loc) override;
	vector getSize() override;
	FgoldenFishStruct getGoldenFish();
private:
	FgoldenFishStruct goldenFish;
	void timerFinished();
	void timerUpdate();
	DeferredPtr<Timer> lifeTimer;

	std::unique_ptr<UprogressBar> progressBar;
	std::unique_ptr<Image> img;
	std::unique_ptr<UhoverBox> hoverBox;
};