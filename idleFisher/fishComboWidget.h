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

	FfishData currFish;
	int quality;

	void updateComboSize();

	void draw(Shader* shaderProgram);
	void setupRandomCombo();
	int getCombo();

	float getValidWidth(Image* img);

	std::unique_ptr<Image> greenImg;
	std::unique_ptr<Image> yellowImg;
	std::unique_ptr<Image> backgroundImg;
	std::unique_ptr<Image> greenWhiteLeft;
	std::unique_ptr<Image> greenWhiteRight;
	std::unique_ptr<Image> yellowWhiteLeft;
	std::unique_ptr<Image> yellowWhiteRight;

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