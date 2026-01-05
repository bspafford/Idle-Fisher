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
	fishComboBorderImg->SetPivot(vector{ 0.5f, 0.f });
	fishImg = std::make_unique<Image>(fishPath, vector{ 0, 0 }, false);
	fishImg->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishImg->SetPivot(vector{ 0.5f, 0.5f });

	greenRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.4510, 1.0, 0.0, 1.0));
	yellowRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, vector{ 0, 0 }, false, glm::vec4(0.8863, 1.0, 0.0, 1.0));
	backgroundRect = std::make_unique<URectangle>(this, vector{ 0, 0 }, fishComboBorderImg->getSize() - 14.f, false, glm::vec4(0.3608, 0.4980, 0.6000, 1.f));

	shake = std::make_unique<Eshake>(0);

	setVisibility(false);
}

void UfishComboWidget::SetFish(FfishData fish, int quality) {
	currFish = fish;
	this->quality = quality;

	updateComboSize();
	fishSpeed = upgrades::calcFishComboSpeed(currFish, quality);
	std::cout << "fish speed: " << fishSpeed << "\n";

	setupRandomCombo();
}

void UfishComboWidget::Start() {
	setVisibility(true);
	fishMoveBack = false;
	fishLoc = 0;
	shake->start({ 0, 0 });
	clickedThisBounce = false;
}

void UfishComboWidget::Stop() {
	setVisibility(false);
	fishMoveBack = false;
	clickedThisBounce = false;
}

int UfishComboWidget::getCombo() {
	clickedThisBounce = true;

	// if going left then the left side of the image needs to be over the combo section
	// if going right then the right side of the image needs to be over the combo section
	int fishX = fishImg->getAbsoluteLoc().x;
	if (!fishMoveBack)
		fishX += fishImg->getSize().x;

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

	float moveDir = fishMoveBack ? -1 : 1;
	fishLoc += fishSpeed * deltaTime * moveDir;
	fishLoc = math::clamp(fishLoc, 0.f, 1.f);

	bool hitWall = false;
	if (fishLoc >= 1 && !fishMoveBack) {
		fishMoveBack = true;
		hitWall = true;
	} else if (fishLoc <= 0 && fishMoveBack) {
		fishMoveBack = false;
		hitWall = true;
	}

	if (hitWall) {
		if (!clickedThisBounce) {
			GetCharacter()->IncreaseCombo(-upgrades::calcComboDecreaseOnBounce());
			Main::comboWidget->spawnComboNumber();
		} else
			clickedThisBounce = false;
	}

	shake->setShakeDist(GetCharacter()->GetCombo() / 2.f);
}

void UfishComboWidget::setupRandomCombo() {
	float yellowRand = math::randRange(0.f, 1.f);
	float greenRand = math::randRange(0.f, 1.f);

	float validYellow = getValidWidth(yellowRect.get());
	float validGreen = getValidWidth(greenRect.get());

	yellowLoc = math::lerp(0.f, validYellow, yellowRand);
	greenLoc = math::lerp(0.f, validGreen, greenRand);
}

void UfishComboWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	vector shakeLoc = shake->getShakeLoc();

	float maxFishX = fishComboBorderImg->getSize().x / 2.f - 15;
	float minFishX = -fishComboBorderImg->getSize().x / 2.f + 15;

	fishComboBorderImg->setLoc(vector{ 0.f, -63.f } + shakeLoc);

	float y = sin(fishLoc * 4.f * M_PI) * 3.f;
	fishImg->setLoc(vector{ math::lerp(minFishX, maxFishX, fishLoc), fishComboBorderImg->getLoc().y + fishComboBorderImg->getSize().y / 2.f + y});
	fishImg->setRotation(-cos(fishLoc * 4.f * M_PI) * 9.f);

	backgroundRect->setLoc(fishComboBorderImg->getAbsoluteLoc() + 7.f);
	yellowRect->setLoc(backgroundRect->getAbsoluteLoc() + vector{ yellowLoc, 0.f });
	greenRect->setLoc(backgroundRect->getAbsoluteLoc() + vector{ greenLoc, 0.f });

	backgroundRect->draw(shaderProgram);
	yellowRect->draw(shaderProgram);
	greenRect->draw(shaderProgram);
	
	fishImg->draw(shaderProgram);
	fishComboBorderImg->draw(shaderProgram);

	fishImg->flipHoizontally(fishMoveBack);
}

void UfishComboWidget::updateComboSize() {
	greenRect->setSize({ math::clamp(calcGreenSize(), 0.f, 1.f) * getValidWidth(nullptr), backgroundRect->getSize().y });
	yellowRect->setSize({ math::clamp(calcYellowSize(), 0.f, 1.f) * getValidWidth(nullptr), backgroundRect->getSize().y });
}

float UfishComboWidget::calcGreenSize() {
	return upgrades::calcGreenFishingUpgrade() / 100.f * (upgrades::calcFishingRodPower() / currFish.greenDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);
}

float UfishComboWidget::calcYellowSize() {
	return calcGreenSize() + upgrades::calcYellowFishingUpgrade() * 2.f / 100.f * (upgrades::calcFishingRodPower() / currFish.yellowDifficulty) * (-0.1 * GetCharacter()->GetCombo() + 1.1);
}

float UfishComboWidget::getValidWidth(URectangle* rect) {
	if (rect) return backgroundRect->getSize().x - rect->getSize().x;
	return backgroundRect->getSize().x;
}