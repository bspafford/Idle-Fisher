#pragma once

#include "widget.h"

class timer;
class animation;
struct FfishData;
class Image;
struct FachievementStruct;
class text;

class UfishUnlocked : public widget {
public:
	UfishUnlocked(widget* parent);
	~UfishUnlocked();
	void draw(Shader* shaderProgram);

	void start(FfishData fish);
	void start(FachievementStruct* achievement);
	void setupLocs() override;

private:
	void reverse();
	void finished();

	std::unique_ptr<Image> bannerImg;
	std::unique_ptr<animation> anim;
	std::unique_ptr<Image> thumbnail;
	std::unique_ptr<text> name;

	std::unique_ptr<timer> finishedTimer;
};