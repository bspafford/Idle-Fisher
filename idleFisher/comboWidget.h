#pragma once

#include "widget.h"
#include "shake.h"

class text;

class UcomboWidget : public widget {
public:
	UcomboWidget(widget* parent);
	~UcomboWidget();

	void spawnComboNumber();

	double prevComboNum = 1;

	void showComboText();
	void hideComboText();

	bool fading = false;
	float fadeTimerMax = .4f;
	float fadeTimer = 0;

	void update(float deltaTime); // native tick for some reason makes play animation not work
	void draw(Shader* shaderProgram);

	std::unique_ptr<text> comboText;
	std::unique_ptr<Eshake> shake;
};