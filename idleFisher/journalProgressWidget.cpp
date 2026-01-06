#include "journalProgressWidget.h"
#include "Input.h"
#include "text.h"

#include "debugger.h"

UjournalProgressWidget::UjournalProgressWidget(widget* parent, int fishNumTotal, int starsNumTotal, int sizeNumTotal) : widget(parent){
	this->fishNumTotal = fishNumTotal;
	this->starsNumTotal = starsNumTotal;
	this->sizeNumTotal = sizeNumTotal;

	background = std::make_unique<Image>("images/widget/journalTiny.png", vector{ 0, 0 }, false);

	fishNumText = std::make_unique<text>(this, "Unlocked", "straightDark", vector{ 0, 0 });
	starsNumText = std::make_unique<text>(this, "Stars", "straightDark", vector{ 0, 0 });
	sizeNumText = std::make_unique<text>(this, "Size", "straightDark", vector{ 0, 0 });

	fishNumTextNum = std::make_unique<text>(this, "0/5", "straightDark", vector{ 0, 0 });
	starsNumTextNum = std::make_unique<text>(this, "0/15", "straightDark", vector{ 0, 0 });
	sizeNumTextNum = std::make_unique<text>(this, "0/5", "straightDark", vector{ 0, 0 });
}

void UjournalProgressWidget::draw(Shader* shaderProgram) {
	vector mousePos = Input::getMousePos();
	background->setLoc(mousePos - vector{ 0, background->getSize().y });
	background->draw(shaderProgram);

	float offset = -9;
	fishNumTextNum->setLoc(mousePos + vector{ 6.f, -20.f + offset });
	starsNumTextNum->setLoc(mousePos + vector{ 6.f, -13.f + offset });
	sizeNumTextNum->setLoc(mousePos + vector{ 6.f, -6.f + offset });

	// gets the biggest text to push back the text by
	float max = math::max(fishNumTextNum->getSize().x, starsNumTextNum->getSize().x);
	max = math::max(max, sizeNumTextNum->getSize().x);

	fishNumText->setLoc(mousePos + vector{ max + 7.f, (-20.f + offset)});
	starsNumText->setLoc(mousePos + vector{ max + 7.f, (-13.f + offset)});
	sizeNumText->setLoc(mousePos + vector{ max + 7.f, (-6.f + offset)});
	
	fishNumText->draw(shaderProgram);
	starsNumText->draw(shaderProgram);
	sizeNumText->draw(shaderProgram);

	fishNumTextNum->draw(shaderProgram);
	starsNumTextNum->draw(shaderProgram);
	sizeNumTextNum->draw(shaderProgram);
}

void UjournalProgressWidget::updateNumbers(int fishNum, int starsNum, int sizeNum) {
	this->fishNum = fishNum;
	this->starsNum = starsNum;
	this->sizeNum = sizeNum;

	std::string unlock = std::to_string(fishNum) + "/" + std::to_string(fishNumTotal);
	std::string stars = std::to_string(starsNum) + "/" + std::to_string(starsNumTotal);
	std::string size = std::to_string(sizeNum) + "/" + std::to_string(sizeNumTotal);
	if (fishNumTextNum->getString() != unlock)
		fishNumTextNum->setText(unlock);
	if (starsNumTextNum->getString() != stars)
		starsNumTextNum->setText(stars);
	if (sizeNumTextNum->getString() != size)
		sizeNumTextNum->setText(size);
}