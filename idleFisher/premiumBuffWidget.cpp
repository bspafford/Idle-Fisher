#include "premiumBuffWidget.h"

#include "main.h"
#include "timer.h"
#include "progressBar.h"
#include "UIWidget.h"
#include "math.h"
#include "hoverBox.h"

#include "debugger.h"

UpremiumBuffWidget::UpremiumBuffWidget(widget* parent, FgoldenFishStruct goldenFish) : widget(parent) {
	this->goldenFish = goldenFish;

	lifeTimer = CreateDeferred<Timer>();
	lifeTimer->addCallback(this, &UpremiumBuffWidget::timerFinished);
	lifeTimer->addUpdateCallback(this, &UpremiumBuffWidget::timerUpdate);
	lifeTimer->start(goldenFish.time);

	progressBar = std::make_unique<UprogressBar>(this, vector{ 15.f, 15.f }, false, PROGRESSBAR_DIR_UP, true);
	progressBar->setBackgroundColor(glm::vec4(0));
	progressBar->setForegroundColor(glm::vec4(0, 0, 0, 0.2));

	img = std::make_unique<Image>("images/fish/premium.png", vector{ 0.f, 0.f }, false);
	hoverBox = std::make_unique<UhoverBox>(this);
	hoverBox->setInfo(goldenFish.name, goldenFish.description);
}

void UpremiumBuffWidget::draw(Shader* shaderProgram) {
	progressBar->draw(shaderProgram);
	img->draw(shaderProgram);

	if (mouseOver()) {
		hoverBox->draw(shaderProgram);
		setHoveredItem(this);
	}
}

void UpremiumBuffWidget::timerFinished() {
	// remove buff
	for (int i = 0; i < Main::premiumBuffList.size(); i++) {
		if (this == Main::premiumBuffList[i].get()) {
			Main::premiumBuffList.erase(Main::premiumBuffList.begin() + i);
		}
	}
	Main::UIWidget->setupLocs();
}

void UpremiumBuffWidget::timerUpdate() {
	progressBar->setPercent(lifeTimer->getTime() / lifeTimer->getMaxTime());
}

void UpremiumBuffWidget::setLoc(vector loc) {
	widget::setLoc(loc);

	progressBar->setLoc(loc);
	vector progressBarSize = progressBar->getSize();
	vector imgLoc = loc + progressBarSize / 2.f - img->getSize() / 2.f;
	img->setLoc(imgLoc);
}

FgoldenFishStruct UpremiumBuffWidget::getGoldenFish() {
	return goldenFish;
}

vector UpremiumBuffWidget::getSize() {
	return vector(17.f, 17.f);
}