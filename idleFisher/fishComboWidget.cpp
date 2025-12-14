#include "fishComboWidget.h"

#include "stuff.h"
#include "main.h"
#include "character.h"
#include "upgrades.h"
#include "petBuffs.h"
#include "baitBuffs.h"

#include "shake.h"

#include "debugger.h"

UfishComboWidget::UfishComboWidget(widget* parent) : widget(parent) {
	std::string fishComboBorderPath = "./images/widget/fishComboBorder.png";
	std::string fishPath = "./images/widget/fish.png";

	fishComboBorderImg = std::make_unique<Image>(fishComboBorderPath, vector{ 0, 0 }, false);
	fishComboBorderImg->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);
	fishImg = std::make_unique<Image>(fishPath, vector{ 0, 0 }, false);
	fishImg->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);

	greenRect = std::make_unique<URectangle>(vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.4510, 1.0, 0.0, 1.0));
	greenRect->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);
	yellowRect = std::make_unique<URectangle>(vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.8863, 1.0, 0.0, 1.0));
	yellowRect->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);
	backgroundRect = std::make_unique<URectangle>(vector{ 0, 0 }, fishComboBorderImg->getSize() - vector{ 10, 10 } * stuff::pixelSize, false, glm::vec4(0.3608, 0.4980, 0.6000, 1.0));
	backgroundRect->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);

	shake = std::make_unique<Eshake>(0);
}

UfishComboWidget::~UfishComboWidget() {

}

int UfishComboWidget::click(bool fishing) {
	// see if you caught good fish
	int combo = getCombo();
	return combo;
}

void UfishComboWidget::Start(FfishData fish, int quality) {
	currFish = fish;
	this->quality = quality;

	shake->start(stuff::screenSize);

	updateComboSize();
	fishSpeed = upgrades::calcFishComboSpeed(currFish, quality);

	visible = true;
	fishMoveBack = false;
	fishLoc = 0;
	setupRandomCombo();
}

int UfishComboWidget::getCombo() {
	int fishX = fishImg->getLoc().x + fishImg->getSize().x / 2;

	int minGreen = greenRect->getLoc().x;
	int maxGreen = greenRect->getLoc().x + greenRect->getSize().x;

	int minYellow = yellowRect->getLoc().x;
	int maxYellow = yellowRect->getLoc().x + yellowRect->getSize().x;

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
	updateComboStartEnd();
	comboLoc = math::lerp(comboStart, comboEnd, num);
}

void UfishComboWidget::updateComboStartEnd() {
	float halfSize = getValidWidth() / 2.f;
	comboStart = -halfSize + yellowRect->getSize().x / 2.f;
	comboEnd = halfSize - yellowRect->getSize().x / 2.f;
}

void UfishComboWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	vector shakeLoc = shake->getShakeLoc();

	float maxFishX = fishComboBorderImg->getSize().x / 2.f - 15 * stuff::pixelSize;
	float minFishX = -fishComboBorderImg->getSize().x / 2.f + 15 * stuff::pixelSize;

	fishComboBorderImg->setLoc(vector{0, 40 * stuff::pixelSize} + shakeLoc);
	float y = sin(fishLoc * 4.f * M_PI) * 3.f;
	fishImg->setLoc(vector{ math::lerp(minFishX, maxFishX, fishLoc) + shakeLoc.x, fishComboBorderImg->getLoc().y + y });
	fishImg->setRotation(-cos(fishLoc * 4.f * M_PI) * 9.f);
	backgroundRect->setLoc(fishComboBorderImg->getLoc());
	yellowRect->setLoc(vector{ float(comboLoc) + shakeLoc.x, backgroundRect->getLoc().y });
	greenRect->setLoc(yellowRect->getLoc());

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
	return fishComboBorderImg->getSize().x - 8 * stuff::pixelSize;
}