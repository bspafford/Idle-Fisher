#pragma once

#include "widget.h"

class AautoFisher;
class Ubutton;
class text;

class AFmoreInfoUI : public widget {
public:
	AFmoreInfoUI(widget* parent, AautoFisher* autoFisher);
	~AFmoreInfoUI();

	void draw(Shader* shaderProgram) override;

	void addToViewport(widget* parent) override;

	void updateUI();

	void setupLocs() override;

private:
	AautoFisher* autoFisher;

	std::unique_ptr<Image> background;
	std::unique_ptr<Image> fullnessBar;
	std::unique_ptr<Image> levelBar;

	std::unique_ptr<text> fisherNum;
	std::unique_ptr<text> levelText;
	std::unique_ptr<text> fullnessText;
	std::unique_ptr<text> fpsText;
	std::unique_ptr<text> mpsText;

	std::unique_ptr<Ubutton> closeButton;
};