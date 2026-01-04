#include "fishComboWidget.h"

#include "main.h"
#include "character.h"
#include "upgrades.h"
#include "Rectangle.h"
#include "shake.h"

#include "debugger.h"

UfishComboWidget::UfishComboWidget(widget* parent) : widget(parent) {
	std::string fishComboBorderPath = "./images/widget/fishComboBorder.png";
	std::string fishPath = "./images/widget/fish.png";

	fishComboBorderImg = std::make_unique<Image>(fishComboBorderPath, vector{ 0, 0 }, false);
	fishComboBorderImg->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishComboBorderImg->SetPivot(vector{ 0.5f, 0.5f });
	fishImg = std::make_unique<Image>(fishPath, vector{ 0, 0 }, false);
	fishImg->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishImg->SetPivot(vector{ 0.5f, 0.5f });

	greenRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.4510, 1.0, 0.0, 1.0));
	greenRect->SetPivot({ 0.5f, 0.f });
	yellowRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.8863, 1.0, 0.0, 1.0));
	backgroundRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, fishComboBorderImg->getSize() - 10.f, false, glm::vec4(0.3608, 0.4980, 0.6000, 1.0));

	shake = std::make_unique<Eshake>(0);

	setVisibility(false);
}

UfishComboWidget::~UfishComboWidget() {

}

int UfishComboWidget::click(bool fishing) {
	return getCombo();
}

void UfishComboWidget::Start(FfishData fish, int quality) {
	currFish = fish;
	this->quality = quality;

	shake->start({ 0, 0 });

	updateComboSize();
	fishSpeed = upgrades::calcFishComboSpeed(currFish, quality);

	setVisibility(true);
	fishMoveBack = false;
	fishLoc = 0;
	setupRandomCombo();
}

int UfishComboWidget::getCombo() {
	int fishX = fishImg->getAbsoluteLoc().x + fishImg->getSize().x / 2.f;

	int minGreen = greenRect->getAbsoluteLoc().x;
	int maxGreen = greenRect->getAbsoluteLoc().x + greenRect->getSize().x;

	int minYellow = yellowRect->getAbsoluteLoc().x;
	int maxYellow = yellowRect->getAbsoluteLoc().x + yellowRect->getSize().x;

	if (fishX >= minGreen && fishX <= maxGreen)
		return 2;
	else if (fishX >= minYellow && fishX <= maxYellow)
		return 1;
	else
		return 0;
}

void UfishComboWidget::Update(float deltaTime) {
	if (!visible)
		return;

	float num = 1;
	if (fishMoveBack)
		num = -1;

	fishLoc += fishSpeed * deltaTime * num;

	bool hitWall = false;
	if (fishLoc >= 1 && !fishMoveBack) {
		fishMoveBack = true;
		hitWall = true;
	} else if (fishLoc <= 0 && fishMoveBack) {
		fishMoveBack = false;
		hitWall = true;
	}

	if (hitWall) {
		GetCharacter()->IncreaseCombo(-upgrades::calcComboDecreaseOnBounce());
		Main::comboWidget->showComboText();
		Main::comboWidget->spawnComboNumber(GetCharacter()->GetCombo());
	}

	shake->setShakeDist(GetCharacter()->GetCombo() / 2.f);
}

void UfishComboWidget::setupRandomCombo() {
	float num = math::randRange(0.f, 1.f);

	float comboEnd = getValidWidth() - yellowRect->getSize().x;

	comboLoc = math::lerp(0.f, comboEnd, num);
}

void UfishComboWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	vector shakeLoc = shake->getShakeLoc();

	float maxFishX = fishComboBorderImg->getSize().x / 2.f - 15;
	float minFishX = -fishComboBorderImg->getSize().x / 2.f + 15;

	fishComboBorderImg->setLoc(vector{ 0.f, -40.f } + shakeLoc);
	float y = sin(fishLoc * 4.f * M_PI) * 3.f;
	fishImg->setLoc(vector{ math::lerp(minFishX, maxFishX, fishLoc), fishComboBorderImg->getLoc().y + y });
	fishImg->setRotation(-cos(fishLoc * 4.f * M_PI) * 9.f);
	backgroundRect->setLoc(fishComboBorderImg->getAbsoluteLoc() + 6.f);
	yellowRect->setLoc(backgroundRect->getAbsoluteLoc() + vector{ comboLoc, 0.f });
	greenRect->setLoc(yellowRect->getAbsoluteLoc() + vector{ yellowRect->getSize().x / 2.f, 0.f });

	backgroundRect->draw(shaderProgram);
	yellowRect->draw(shaderProgram);
	greenRect->draw(shaderProgram);
	
	fishImg->draw(shaderProgram);
	fishComboBorderImg->draw(shaderProgram);

	fishImg->flipHoizontally(fishMoveBack);
}

void UfishComboWidget::updateComboSize() {
	greenRect->setSize({ math::clamp(calcGreenSize(), 0, 1) * getValidWidth(), backgroundRect->getSize().y });
	yellowRect->setSize({ math::clamp(calcYellowSize(), 0, 1) * getValidWidth(), backgroundRect->getSize().y });
}

float UfishComboWidget::calcGreenSize() {
	return upgrades::calcGreenFishingUpgrade() / 100.f * (1.f / currFish.greenDifficulty * upgrades::calcFishingRodPower());
}

float UfishComboWidget::calcYellowSize() {
	return calcGreenSize() + upgrades::calcYellowFishingUpgrade() * 2.f / 100.f * (1.f / currFish.yellowDifficulty * upgrades::calcFishingRodPower());
}

float UfishComboWidget::getValidWidth() {
	return fishComboBorderImg->getSize().x - 10.f;
}