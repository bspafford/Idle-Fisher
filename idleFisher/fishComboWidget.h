#pragma once

#include <string>

#include "saveData.h"
#include "widget.h"

class Shader;
class Image;
class URectangle;
class Eshake;

class UfishComboWidget : public widget {
public:
	UfishComboWidget(widget* parent);

	void SetFish(FfishData fish, int quality);
	void Start();
	void Stop();
	void Update(float deltaTime);

	float calcYellowSize();
	float calcGreenSize();
	FfishData currFish;
	int quality;

	void updateComboSize();

	void draw(Shader* shaderProgram);
	void setupRandomCombo();
	int getCombo();

	float getValidWidth(URectangle* rect);

	std::unique_ptr<URectangle> greenRect;
	std::unique_ptr<URectangle> yellowRect;
	std::unique_ptr<URectangle> backgroundRect;

	float yellowLoc = 0.f;
	float greenLoc = 0.f;

	float fishLoc = 0.f; // percent of its location on bar
	float fishSpeed = 0.f;

	// widget components
	std::unique_ptr<Image> fishComboBorderImg;
	int fishComboBorderX, fishComboBorderY;

	std::unique_ptr<Image> fishImg;

	// should move backwards or forwards
	bool fishMoveBack = false;

	std::unique_ptr<Eshake> shake;

	// if the player has clicked from since they last hit the wall to the next 
	bool clickedThisBounce = false;
};