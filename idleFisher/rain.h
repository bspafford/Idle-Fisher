#pragma once

#include "math.h"
#include "deferredPtr.h"

class Timer;
class animation;
class Shader;

class Arain {
public:
	Arain();
	~Arain();
	void draw(Shader* shaderProgram);
	void start();
	void stop();
private:
	void updateRain();
	void removeAnimList();

	std::vector<std::string> animImgList;

	std::unique_ptr<Image> rainDrop;
	DeferredPtr<Timer> rainTimer;
	std::vector<std::unique_ptr<animation>> animList;

	// temp
	std::vector<vector> locs;
	std::vector<float> deathLoc; // what y the raindrop hits the ground

	bool isStopped = false;
};