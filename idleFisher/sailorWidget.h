#pragma once

#include "widget.h"
#include "deferredPtr.h"

class text;
class Ubutton;
class Umap;
class animation;
class Timer;

class UsailorWidget : public widget {
public:
	UsailorWidget(widget* parent);
	~UsailorWidget();


	void draw(Shader* shaderProgram) override;

	std::unique_ptr<Image> mapBackground;

private:
	void addedToViewport() override;
	void closeWidget();

	std::unique_ptr<Umap> map;
	std::unique_ptr<animation> mapAnim;
	std::unique_ptr<Image> mapClosed;

	std::unique_ptr<Ubutton> xButton;

	bool mouseDown = false;

	vector mouseStartPos;
	vector imgStartPos;

	void setupLocs() override;

	DeferredPtr<Timer> mapTimer;
	void moveAnim();
	void mapTimerFinish();
	void mapAnimFinish();
	void removeFromViewport() override;

	bool opening = false;
	bool closing = false;
	// if journal is all the way open
	bool open = false;
};
