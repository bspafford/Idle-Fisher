#include "loadingScreen.h"
#include "timer.h"
#include "text.h"

#include "debugger.h"

LoadingScreen::LoadingScreen(widget* parent) : widget(parent) {
	loadingText = std::make_unique<text>(this, "Loading...", "afScreen", vector{ 0, 0 });
	loadingTextTimer = CreateDeferred<Timer>();
	loadingTextTimer->addCallback(this, &LoadingScreen::loadingTimerCallback);
	loadingTextTimer->start(0.25f);
	setupLocs();
}

void LoadingScreen::draw(Shader* shaderProgram) {
	loadingText->draw(shaderProgram);
}
	
void LoadingScreen::loadingTimerCallback() {
	std::string periods[4] = { "", ".", "..", "..." };
	loadingText->setText("Loading" + periods[periodNum]);
	loadingTextTimer->start(0.25f);
	periodNum = (periodNum + 1) % 4;
}

void LoadingScreen::setupLocs() {
	loadingText->setLoc(vector{ stuff::screenSize.x / stuff::pixelSize - 135.f, 50.f });
}