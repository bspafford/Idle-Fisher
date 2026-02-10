#pragma once

#include "widget.h"
#include "Audio.h"

class AautoFisher;
class AFmoreInfoUI;
class animation;
class Ubutton;
class text;
class Image;

class autoFisherUI : public widget {
public:
	autoFisherUI(widget* parent, AautoFisher* autoFisherRef, vector loc);
	~autoFisherUI();

	void draw(Shader* shaderProgram);

	void updateUI();
	void openUI();
	void closeUI();

	bool visible = false;

	bool opening, closing;

private:
	void upgrade();

	void openedUI();
	void closedUI();

	void moveButtonText(int frame);
	vector buttonTextLoc;

	bool UIOpen;

	AautoFisher* autoFisher;

	std::unique_ptr<Ubutton> upgradeButton;
	// Image fullness;
	//Image multiplierButton;
	std::unique_ptr<Ubutton> multi1;
	std::unique_ptr<Ubutton> multi10;
	std::unique_ptr<Ubutton> multiMax;
	std::unique_ptr<Image> pressTest;

	std::unique_ptr<Ubutton> menuButton;
	void openMenu();

	std::unique_ptr<Image> levelBar;

	std::unique_ptr<text> levelText;
	std::unique_ptr<text> maxText; // text show when player upgrades multi is MAX
public:
	std::unique_ptr<text> buttonText;
private:
	std::vector<std::string> levelBarImgs;

	std::unique_ptr<Audio> openAudio;

	vector UILoc;

	std::unique_ptr<animation> openAnimation;

	void setMultiplier(int multiplier);
	void setMulti1();
	void setMulti10();
	void setMultiMax();

	std::vector<std::string> multi1Anim;

	double upgradeCost = 0.0;
};