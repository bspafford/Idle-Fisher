#include "fps.h"
#include "timer.h"
#include "main.h"

#include <iostream>

#include "debugger.h"

fps::fps() {
	fpsTimer = std::make_unique<timer>();
	fpsTimer->addCallback(fps::returnTimer);
	fpsTimer->start(.1);

	outlierTimer = std::make_unique<timer>();
	outlierTimer->addCallback(fps::returnTimerOutlier);
	outlierTimer->start(.1);
}

void fps::update(float deltaTime) {
	if (!bShow)
		return;

	num++;
	total += deltaTime;

	float fps = 1.f / deltaTime;
	if (Main::GetFPSLimit() == 0 && fps < 1000.f)
		std::cout << "low fps: " << fps << std::endl;
}

void fps::returnTimer() {
	if (bShow) {
		float avg = total / num;
		std::cout << "fps: " << 1 / avg << std::endl;
	}

	num = 0;
	total = 0;

	fpsTimer->start(.1);
}

void fps::returnTimerOutlier() {
	//if (bShow) {
	//	float avg = totalOutlier / numOutlier;
	//	std::cout << "fps: " << 1 / avg << std::endl;
	//}

	numOutlier = 0;
	totalOutlier = 0;

	outlierTimer->start(.1);
}

void fps::showFPS(bool show) {
	bShow = show;
}

void fps::outliers(float deltaTime) {
	numOutlier++;
	totalOutlier += deltaTime;

	float q1 = totalOutlier / numOutlier * .25;
	float q3 = totalOutlier / numOutlier * .75;
	float iqr = q3 - q1;

	float upperBound = q3 + 1.5 * iqr;

	if (deltaTime > upperBound)
		std::cout << "lag spike: " << 1 / deltaTime << std::endl;
}