#pragma once

#include "math.h"
#include "deferredPtr.h"

class Shader;
class animation;
class Timer;
class Image;

class AfishSchool {
public:
	AfishSchool();

	void Draw(Shader* shaderProgram);
	bool PointInSchool(vector worldPoint);

private:
	void Start();
	void Finished();

	vector GetRandomLoc();

	std::unique_ptr<animation> anim;
	DeferredPtr<Timer> spawnTimer;
	DeferredPtr<Timer> lifeTimer;

	bool isAlive = false; // if the fish school is actually active
};
