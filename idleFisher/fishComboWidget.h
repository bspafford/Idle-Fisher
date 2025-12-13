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
	~UfishComboWidget();

	int click(bool fishing);
	void Start(FfishData fish, int quality);
	void Update(float deltaTime);

	float calcYellowSize();
	float calcGreenSize();
	FfishData currFish;
	int quality;

	void updateComboSize();

	void draw(Shader* shaderProgram);
	void setupRandomCombo();
	void updateComboStartEnd();
	int getCombo();

	float getValidWidth();

	std::unique_ptr<URectangle> greenRect;
	std::unique_ptr<URectangle> yellowRect;
	std::unique_ptr<URectangle> backgroundRect;

	int comboStart = 0, comboEnd = 0;
	int comboLoc = 0;

	bool visible = false;

	float fishLoc = 0; // percent of its location on bar
	float fishSpeed = 0;

	// widget components
	std::unique_ptr<Image> fishComboBorderImg;
	int fishComboBorderX, fishComboBorderY;

	std::unique_ptr<Image> fishImg;

	// should move backwards or forwards
	bool fishMoveBack = false;
	int hitWallNum = 0;

	std::unique_ptr<Eshake> shake;
};