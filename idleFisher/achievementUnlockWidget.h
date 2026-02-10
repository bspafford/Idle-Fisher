#pragma once

#include "widget.h"
#include "deferredPtr.h"
#include "Audio.h"

class Timer;
class animation;
struct FfishData;
class Image;
struct FachievementStruct;
class text;

class UachievementUnlockWidget : public widget {
public:
	UachievementUnlockWidget(widget* parent);
	~UachievementUnlockWidget();
	void draw(Shader* shaderProgram);

	void start(const FachievementStruct& achievement);
	void setupLocs() override;

private:
	void reverse();
	void finished();

	std::unique_ptr<Image> bannerImg;
	std::unique_ptr<animation> anim;
	std::unique_ptr<Image> thumbnail;
	std::unique_ptr<text> name;

	DeferredPtr<Timer> finishedTimer;
	std::unique_ptr<Audio> unlockAudio;
};