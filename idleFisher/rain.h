#pragma once

#include "math.h"
#include "deferredPtr.h"

class Timer;
class animation;
class Shader;
class URectangle;

class Arain {
public:
	Arain();

	void Draw(Shader* shaderProgram);
	void Start();
	void Stop();
	bool IsRaining();
private:
	void UpdateRain();
	void DarkenScreen();

	std::vector<std::string> animImgList;

	std::unique_ptr<URectangle> darkBackground;

	std::unique_ptr<Image> rainDrop;
	DeferredPtr<Timer> rainTimer;
	std::vector<std::unique_ptr<animation>> animList;

	DeferredPtr<Timer> darkenScreenTimer;

	// temp
	std::vector<vector> locs;
	std::vector<float> deathLoc; // what y the raindrop hits the ground

	bool isStopped = false;
	bool raining = false;
};