#pragma once

#include "widget.h"

class text;

class UcomboWidget : public widget {
public:
	UcomboWidget(widget* parent);
	~UcomboWidget();

	void setupLocs() override;

	void tempIncrease();
	void tempSame();
	void tempBreak();

	void spawnComboNumber(double comboNum);

	double prevComboNum = 1;

	void showComboText();
	void hideComboText();

	bool fading = false;
	float fadeTimerMax = .4f;
	float fadeTimer = 0;

	void update(float deltaTime); // native tick for some reason makes play animation not work
	void draw(Shader* shaderProgram);

	std::unique_ptr<text> comboText;
};