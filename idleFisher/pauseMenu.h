#pragma once

#include "widget.h"

class Image;
class Ubutton;
class text;
class verticalBox;
class Usettings;
class BlurBox;

class UpauseMenu : public widget {
public:
	UpauseMenu(widget* parent);
	~UpauseMenu();
	void draw(Shader* shaderProgram);
private:
	void setupLocs() override;

	void resume();
	void saveGame();
	void settings();
	void exitToMenu();
	void exitToDesktop();

	std::unique_ptr<URectangle> rect;
	std::unique_ptr<BlurBox> blurBox;
	std::unique_ptr<Image> pauseText;
	std::unique_ptr<Ubutton> continueButton;
	std::unique_ptr<Ubutton> saveGameButton;
	std::unique_ptr<Ubutton> settingsButton;
	std::unique_ptr<Ubutton> exitToMenuButton;
	std::unique_ptr<Ubutton> exitToDesktopButton;
	std::unique_ptr<verticalBox> vertBox;

	std::unique_ptr<Usettings> settingsWidget;
};