#pragma once

#include "widget.h"
#include "deferredPtr.h"

class text;
class UpremiumBuffWidget;
class UprogressBar;
class Ubutton;
class Timer;

class UUIWidget : public widget {
public:
	UUIWidget(widget* parent);
	~UUIWidget();
	void draw(Shader* shaderProgram) override;
	void updateProgressBar(double points, double progress, double needed);
	void setupLocs() override;

private:
	std::unique_ptr<UprogressBar> progressBar;
	std::unique_ptr<text> rebirthPointNum;
	std::unique_ptr<text> currRunRebirthPoints; // how many rebirth points you have for that run

	std::unique_ptr<Ubutton> NPCshowButton;

	std::unique_ptr<Ubutton> merchantButton;
	std::unique_ptr<Ubutton> fishermanButton;
	std::unique_ptr<Ubutton> mechanicButton;
	std::unique_ptr<Ubutton> petSellerButton;
	std::unique_ptr<Ubutton> sailorButton;
	std::unique_ptr<Ubutton> atmButton;

	void showNPCButtons();

	// show npc button stuff
	DeferredPtr<Timer> buttonsTimer;
	void updateButtonsLoc();
	void finishButtons();
	std::vector<Ubutton*> buttonsList;
	bool open = false;
	bool canOpenClose = true;

	void openMerchantWidget();
	void openFishermanWidget();
	void openMechanicWidget();
	void openPetSellerWidget();
	void openSailorWidget();
	void openAtmWidget();
};