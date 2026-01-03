#pragma once

#include "math.h"
#include "deferredPtr.h"

class Shader;
class animation;
class Timer;
class Image;

class AfishSchool {
public:
	AfishSchool(vector loc);
	~AfishSchool();
	void draw(Shader* shaderProgram);
	void setLoc(vector loc);
	AfishSchool* pointInSchool(vector worldPoint);
	void removeFishNum();
private:
	void remove();

	vector loc;

	std::unique_ptr<animation> anim;

	// amount of times you can fish from, random
	// alive time, random
	DeferredPtr<Timer> lifeTimer;
	float fishNum;
};
