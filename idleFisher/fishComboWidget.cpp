#include "fishComboWidget.h"

#include "main.h"
#include "character.h"
#include "upgrades.h"
#include "Rectangle.h"
#include "shake.h"

#include "debugger.h"

UfishComboWidget::UfishComboWidget(widget* parent) : widget(parent) {
	std::string fishComboBorderPath = "images/widget/fishComboBorder.png";
	std::string fishPath = "images/widget/fish.png";

	fishComboBorderImg = std::make_unique<Image>(fishComboBorderPath, vector{ 0, 0 }, false);
	fishComboBorderImg->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishComboBorderImg->SetPivot(vector{ 0.5f, 0.f });
	fishImg = std::make_unique<Image>(fishPath, vector{ 0, 0 }, false);
	fishImg->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	fishImg->SetPivot(vector{ 0.5f, 0.5f });

	greenImg = std::make_unique<Image>("images/widget/comboBoxGreen.png", vector{ 0, 0 }, false);
	yellowImg = std::make_unique<Image>("images/widget/comboBoxYellow.png", vector{ 0, 0 }, false);
	backgroundImg = std::make_unique<Image>("images/widget/comboBoxBlue.png", vector{0, 0}, false);
	backgroundImg->setSize(vector(fishComboBorderImg->getSize().x - 14.f, backgroundImg->getSize().y));

	greenWhiteLeft = std::make_unique<Image>("images/widget/comboBoxWhite.png", vector{ 0, 0 }, false);
	greenWhiteRight = std::make_unique<Image>("images/widget/comboBoxWhite.png", vector{ 0, 0 }, false);
	greenWhiteRight->SetPivot(vector(1.f, 0.f));
	yellowWhiteLeft = std::make_unique<Image>("images/widget/comboBoxWhite.png", vector{ 0, 0 }, false);
	yellowWhiteRight = std::make_unique<Image>("images/widget/comboBoxWhite.png", vector{ 0, 0 }, false);
	yellowWhiteRight->SetPivot(vector(1.f, 0.f));

	shake = std::make_unique<Eshake>(0);

	setVisibility(false);
}

void UfishComboWidget::SetFish(FfishData fish, int quality) {
	currFish = fish;
	this->quality = quality;

	updateComboSize();
	fishSpeed = Upgrades::Get(StatContext(Stat::FishComboSpeed, currFish.id, quality));

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

	int minGreen = greenImg->getAbsoluteLoc().x;
	int maxGreen = greenImg->getAbsoluteLoc().x + greenImg->getSize().x;

	int minYellow = yellowImg->getAbsoluteLoc().x;
	int maxYellow = yellowImg->getAbsoluteLoc().x + yellowImg->getSize().x;

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
			// decrease the combo
			GetCharacter()->IncreaseCombo(-Upgrades::Get(Stat::ComboDecreaseOnBounce));

			// update combo number
			Main::comboWidget->spawnComboNumber();

			// recalc the yellow and green size without changing locations
			updateComboSize();
		} else
			clickedThisBounce = false;
	}

	shake->setShakeDist(GetCharacter()->GetCombo() / Upgrades::Get(Stat::ComboMax) / 10.f);
}

void UfishComboWidget::setupRandomCombo() {
	float yellowRand = math::randRange(0.f, 1.f);
	float greenRand = math::randRange(0.f, 1.f);

	float validYellow = getValidWidth(yellowImg.get());
	float validGreen = getValidWidth(greenImg.get());

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

	backgroundImg->setLoc(fishComboBorderImg->getAbsoluteLoc() + 7.f);
	yellowImg->setLoc(backgroundImg->getAbsoluteLoc() + vector{ yellowLoc, 0.f });
	greenImg->setLoc(backgroundImg->getAbsoluteLoc() + vector{ greenLoc, 0.f });

	greenWhiteLeft->setLoc(greenImg->getAbsoluteLoc());
	greenWhiteRight->setLoc(greenImg->getAbsoluteLoc() + vector(greenImg->getSize().x, 0.f));
	yellowWhiteLeft->setLoc(yellowImg->getAbsoluteLoc());
	yellowWhiteRight->setLoc(yellowImg->getAbsoluteLoc() + vector(yellowImg->getSize().x, 0.f));

	backgroundImg->draw(shaderProgram);
	yellowImg->draw(shaderProgram);

	yellowWhiteLeft->draw(shaderProgram);
	yellowWhiteRight->draw(shaderProgram);
	
	greenImg->draw(shaderProgram);
	
	greenWhiteLeft->draw(shaderProgram);
	greenWhiteRight->draw(shaderProgram);


	
	fishImg->draw(shaderProgram);
	fishComboBorderImg->draw(shaderProgram);

	fishImg->flipHoizontally(fishMoveBack);
}

void UfishComboWidget::updateComboSize() {
	greenImg->setSize(vector(math::clamp(Upgrades::Get(StatContext(Stat::GreenComboSize, currFish.id)), 0.0, 1.0) * getValidWidth(nullptr), backgroundImg->getSize().y).round());
	yellowImg->setSize(vector(math::clamp(Upgrades::Get(StatContext(Stat::YellowComboSize, currFish.id)), 0.0, 1.0) * getValidWidth(nullptr), backgroundImg->getSize().y).round());
}



float UfishComboWidget::getValidWidth(Image* img) {
	if (img) return backgroundImg->getSize().x - img->getSize().x;
	return backgroundImg->getSize().x;
}