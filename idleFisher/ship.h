#pragma once

#include "math.h"
#include "deferredPtr.h"

class Shader;
class Timer;

class Aship {
public:
	Aship(vector loc);
	~Aship();
	void draw(Shader* shaderProgram);
private:
	void shipbob();
	DeferredPtr<Timer> bobTimer;
	std::unique_ptr<Image> shipImg;

	vector loc;
	vector tempLoc;
};