#pragma once

#include "widget.h"
#include "deferredPtr.h"

class Timer;
class text;

class LoadingScreen : public widget {
public:
	LoadingScreen(widget* parent);

	void draw(Shader* shaderProgram) override;
	void setupLocs() override;

private:
	std::unique_ptr<text> loadingText;
	DeferredPtr<Timer> loadingTextTimer;
	void loadingTimerCallback();

	int periodNum = 0;
};