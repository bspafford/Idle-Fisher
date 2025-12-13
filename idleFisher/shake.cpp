#include "shake.h"
#include "timer.h"

#include "debugger.h"

Eshake::Eshake(float shakeDist) {
	shakeTimer = std::make_unique<timer>();
	shakeTimer->addCallback(this, &Eshake::setShakeLoc);
	this->shakeDist = shakeDist;
	shakeTimer->start(.1);
}

void Eshake::start(vector loc) {
	shakeLoc = loc;
}

void Eshake::updateShake(vector loc) {
	startLoc = loc;
}

void Eshake::setShakeDist(float shakeDist) {
	this->shakeDist = shakeDist;
}

void Eshake::setShakeLoc() {
	float radius = 5.f;
	float r = radius * sqrt(math::randRange(0.f, 1.f));
	float theta = math::randRange(0.f, 1.f) * 2.f * M_PI;
	float x = startLoc.x + r * cos(theta);
	float y = startLoc.y + r * sin(theta);
	shakeLoc = { x, y };
	shakeTimer->start(0.1f);
}

vector Eshake::getShakeLoc() {
	return shakeLoc;
}