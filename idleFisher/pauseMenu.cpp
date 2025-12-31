#include "pauseMenu.h"
#include "main.h"
#include "saveData.h"
#include "math.h"
#include "settings.h"
#include "button.h"
#include "text.h"
#include "verticalBox.h"
#include "settings.h"
#include "blurBox.h"

#include "debugger.h"

UpauseMenu::UpauseMenu(widget* parent) : widget(parent) {
	rect = std::make_unique<URectangle>(vector{ 0, 0 }, stuff::screenSize, false, glm::vec4(0, 0, 0, 0.1f));
	blurBox = std::make_unique<BlurBox>(this, vector{ 0, 0 }, stuff::screenSize / stuff::pixelSize, 4);
	pauseText = std::make_unique<Image>("./images/widget/pauseMenu/pause.png", vector{ 3, -3 } * stuff::pixelSize, false);
	pauseText->SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	pauseText->SetPivot({ 0, 1 });
	continueButton = std::make_unique<Ubutton>(this, "widget/pauseMenu/continue.png", 69, 20, 1, vector{ 0, 0 }, false, false);
	saveGameButton = std::make_unique<Ubutton>(this, "widget/pauseMenu/saveGame.png", 72, 20, 1, vector{ 0, 0 }, false, false);
	settingsButton = std::make_unique<Ubutton>(this, "widget/pauseMenu/settings.png", 70, 20, 1, vector{ 0, 0 }, false, false);
	exitToMenuButton = std::make_unique<Ubutton>(this, "widget/pauseMenu/exitToMenu.png", 93, 20, 1, vector{ 0, 0 }, false, false);
	exitToDesktopButton = std::make_unique<Ubutton>(this, "widget/pauseMenu/exitGame.png", 74, 20, 1, vector{ 0, 0 }, false, false);
	
	settingsWidget = std::make_unique<Usettings>(nullptr);

	vertBox = std::make_unique<verticalBox>(this);
	vertBox->SetAnchor(ANCHOR_LEFT, ANCHOR_TOP);
	vertBox->SetPivot({ 0, 1 });

	if (vertBox) {
		float padding = 10;
		if (continueButton) {
			continueButton->addCallback(this, &UpauseMenu::resume);
			vertBox->addChild(continueButton.get(), continueButton->getSize().y + padding);
		}
		if (saveGameButton) {
			saveGameButton->addCallback(this, &UpauseMenu::saveGame);
			vertBox->addChild(saveGameButton.get(), saveGameButton->getSize().y + padding);
		}
		if (settingsButton) {
			settingsButton->addCallback(this, &UpauseMenu::settings);
			vertBox->addChild(settingsButton.get(), settingsButton->getSize().y + padding);
		}
		if (exitToMenuButton) {
			exitToMenuButton->addCallback(this, &UpauseMenu::exitToMenu);
			vertBox->addChild(exitToMenuButton.get(), exitToMenuButton->getSize().y + padding);
		}
		if (exitToDesktopButton) {
			exitToDesktopButton->addCallback(this, &UpauseMenu::exitToDesktop);
			vertBox->addChild(exitToDesktopButton.get(), exitToDesktopButton->getSize().y + padding);
		}
	}

	setupLocs();
}

UpauseMenu::~UpauseMenu() {

}

void UpauseMenu::draw(Shader* shaderProgram) {
	rect->draw(shaderProgram);
	blurBox->draw();
	pauseText->draw(shaderProgram);
	vertBox->draw(shaderProgram);
}

void UpauseMenu::setupLocs() {
	vertBox->setLocAndSize({ 20.f, -70.f }, vector{ stuff::screenSize.x / stuff::pixelSize, vertBox->getOverflowSize() });
	
	blurBox->setSize(stuff::screenSize / stuff::pixelSize);
	rect->setLoc(rect->getLoc());
	rect->setSize(stuff::screenSize / stuff::pixelSize);
}

void UpauseMenu::resume() {
	widget::getCurrWidget()->removeFromViewport();
}

void UpauseMenu::saveGame() {
	SaveData::save();
}

void UpauseMenu::settings() {
	settingsWidget->addToViewport(true);
}

void UpauseMenu::exitToMenu() {
	Scene::openLevel("titleScreen");
}

void UpauseMenu::exitToDesktop() {
	glfwSetWindowShouldClose(Main::GetWindow(), true);
}

Usettings* UpauseMenu::GetSettingsWidget() {
	return settingsWidget.get();
}