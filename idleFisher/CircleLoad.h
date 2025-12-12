#pragma once

#include "Rectangle.h"
#include "widget.h"

class timer;

class CircleLoad : public URectangle, public widget {
public:
	CircleLoad();

	void draw();
	void Start();
	void isAnimating();

private:
	static inline std::unique_ptr<Shader> circleShader = NULL;
	
	void NextCircle();
	virtual void setupLocs() override;

	bool animating = false;
	std::unique_ptr<timer> circleTimer;
	int circleIndex;
	float circleRadius[4] = { 0.f, 15.f / 240.f, 64.f / 240.f, 214.f / 240.f };
};