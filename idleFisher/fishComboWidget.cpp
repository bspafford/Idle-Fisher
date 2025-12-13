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
	backgroundRect = std::make_unique<URectangle>(vector{ 0, 0 }, fishComboBorderImg->getSize() - vector{ 4, 4 } *stuff::pixelSize, false, glm::vec4(0.3608, 0.4980, 0.6000, 1.0));
	backgroundRect->setAnchor(IMAGE_ANCHOR_CENTER, IMAGE_ANCHOR_CENTER);


	shake = std::make_unique<Eshake>(5);
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
	hitWallNum = 0;
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

	if (fishLoc >= 1 && !fishMoveBack) {
		fishMoveBack = true;
		hitWallNum++;
	} else if (fishLoc <= 0 && fishMoveBack) {
		fishMoveBack = false;
		hitWallNum++;
	}

	if (hitWallNum > upgrades::calcMaxComboBounce()) {
		// didn't catch fish
		Main::character->comboExceeded();
	}
}

void UfishComboWidget::setupRandomCombo() {
	updateComboStartEnd();
	double num = rand() / RAND_MAX;
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

	shake->updateShake(stuff::screenSize);

	float maxFishX = fishComboBorderImg->getSize().x / 2.f - 7 * stuff::pixelSize;
	float minFishX = -fishComboBorderImg->getSize().x / 2.f + 7 * stuff::pixelSize;
	fishImg->setLoc({ math::lerp(minFishX, maxFishX, fishLoc), fishComboBorderImg->getLoc().y });

	fishComboBorderImg->setLoc({0, 30 * stuff::pixelSize});
	yellowRect->setLoc({ float(comboLoc), backgroundRect->getLoc().y });
	greenRect->setLoc(yellowRect->getLoc());
	backgroundRect->setLoc(fishComboBorderImg->getLoc());

	backgroundRect->draw(shaderProgram);
	yellowRect->draw(shaderProgram);
	greenRect->draw(shaderProgram);
	
	fishImg->draw(shaderProgram);
	fishComboBorderImg->draw(shaderProgram);

	fishImg->flipHoizontally(fishMoveBack);
}

void UfishComboWidget::updateComboSize() {
	std::cout << "greenSize: " << calcGreenSize() << ", yellowSize: " << calcYellowSize() << std::endl;
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
	return fishComboBorderImg->getSize().x - 6 * stuff::pixelSize;
}