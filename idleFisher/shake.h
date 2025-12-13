#pragma once

#include "math.h"
#include "timer.h"

class Eshake {
public:
	Eshake(float shakeDist);
	void setShakeDist(float shakeDist);
	vector getShakeLoc();
	void start(vector loc);

private:
	// called when the timer is finished
	void setShakeLoc();

	float shakeDist;
	vector shakeLoc;

	std::unique_ptr<timer> shakeTimer;
};