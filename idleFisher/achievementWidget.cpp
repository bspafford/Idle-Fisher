#include "achievementWidget.h"

#include "main.h"
#include "saveData.h"
#include "timer.h"

// widgets
#include "achievementBox.h"
#include "wrapBox.h"
#include "scrollBox.h"
#include "hoverBox.h"
#include "button.h"
#include "text.h"
#include "equippedWidget.h"

#include "debugger.h"

UachievementWidget::UachievementWidget(widget* parent) : widget(parent) {
	background = std::make_unique<Image>("./images/widget/achievementBackground.png", vector{ 0, 0 }, false);
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	startTimeText = std::make_unique<text>(this, "", "straight", vector{ 0, 0 });
	changeTextTimer = CreateDeferred<Timer>();
	changeTextTimer->addCallback(this, &UachievementWidget::updateText);
	updateText();

	scrollBox = std::make_unique<UscrollBox>(this);
	scrollBox->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	scrollBox->SetPivot({ 0.5f, 0.5f });

	equippedWidget = std::make_unique<UequippedWidget>(this);
	equippedWidget->setParent(scrollBox.get());
	achievementHolder = std::make_unique<UwrapBox>(this, loc, size);
	achievementText = std::make_unique<text>(this, "Achievements - 0/500 (0%)", "straight", vector{ 0, 0 });
	hoverBox = std::make_unique<UhoverBox>(this);
	xButton = std::make_unique<Ubutton>(this, "widget/npcXButton.png", 11, 11, 1, vector{ 0, 0 }, false, false);
	xButton->SetPivot({ 0.5f, 0.5f });
	xButton->addCallback(this, &UachievementWidget::closeWidget);
	if (scrollBox) {
		scrollBox->addChild(NULL, 2);
		scrollBox->addChild(startTimeText.get(), 7);
		scrollBox->addChild(equippedWidget.get(), equippedWidget->getSize().y);
		scrollBox->addChild(achievementText.get(), achievementText->getSize().y);
		scrollBox->addChild(achievementHolder.get(), 5.f);
	}

	for (int i = 0; i < SaveData::data.achievementData.size(); i++) {
		std::unique_ptr<UachievementBox> achievementBox = std::make_unique<UachievementBox>(achievementHolder.get(), i);
		achievementHolder->addChild(achievementBox.get());
		achievementBoxList.push_back(std::move(achievementBox));
	}

	setupLocs();
}

UachievementWidget::~UachievementWidget() {

}

void UachievementWidget::draw(Shader* shaderProgram) {
	background->draw(shaderProgram);

	scrollBox->draw(shaderProgram);

	for (int i = 0; i < SaveData::data.achievementData.size(); i++) {
		widget* child = achievementHolder->getChildAt(i);
		if (child->mouseOver()) {
			if (SaveData::saveData.achievementList[i].unlocked) {
				hoverBox->setInfo(SaveData::data.achievementData[i].name, SaveData::data.achievementData[i].description);
				hoverBox->draw(shaderProgram);
			}
			break;
		}
	}

	std::string name, description;
	if (equippedWidget->mouseOverSlot(name, description)) {
		hoverBox->setInfo(name, description);
		hoverBox->draw(shaderProgram);
	}

	xButton->draw(shaderProgram);
}

void UachievementWidget::addedToViewport() {
	float achievementsUnlocked = 0.f;
	for (FsaveAchievementStruct achievement : SaveData::saveData.achievementList) {
		if (achievement.unlocked)
			achievementsUnlocked++;
	}

	float totalAchievements = static_cast<float>(SaveData::saveData.achievementList.size());

	// displays 1 decimal or 0 if whole number
	std::string percentAchievements;
	float percent = achievementsUnlocked / totalAchievements * 100.f;
	if (percent == std::floor(percent))
		percentAchievements = std::format("{}", static_cast<int>(percent));
	else
		percentAchievements = std::format("{:.1f}", percent);

	achievementText->setText("Achievements - " + std::to_string(static_cast<int>(achievementsUnlocked)) + "/" + std::to_string(static_cast<int>(totalAchievements)) + " (" + percentAchievements + "%)");
}

void UachievementWidget::setupLocs() {
	__super::setupLocs();

	vector scrollBoxLoc = background->getAbsoluteLoc() + vector{ 11.f, 9.f };
	vector scrollBoxSize = vector{ 17.f * 25.f + 2.f, background->getSize().y - 19.f };
	//scrollBox->setOgLoc(scrollBoxLoc);
	achievementHolder->setLocAndSize(scrollBoxLoc, { scrollBoxSize.x, achievementHolder->getSize().y });
	scrollBox->changeChildHeight(achievementHolder.get(), achievementHolder->getOverflowSize());
	scrollBox->setLocAndSize({ 0.f, 0.f }, scrollBoxSize);

	if (xButton)
		xButton->setLoc(background->getAbsoluteLoc() + background->getSize());
}

void UachievementWidget::updateText() {
	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	int64_t total = std::chrono::duration_cast<std::chrono::seconds>(currentTime - SaveData::startTime).count();

	const int secondsInMinute = 60;
	const int secondsInHour = 3600;
	const int secondsInDay = 86400;

	int days = total / secondsInDay;
	total %= secondsInDay;
	int hours = total / secondsInHour;
	total %= secondsInHour;
	int minutes = total / secondsInMinute;
	total %= secondsInMinute;
	int seconds = total;

	std::string timeString = "";
	if (days > 0)
		timeString.append(std::to_string(days) + " day ");
	if (hours > 0)
		timeString.append(std::to_string(hours) + " hrs ");
	if (minutes > 0)
		timeString.append(std::to_string(minutes) + " min ");
	timeString.append(std::to_string(seconds) + " sec");

	startTimeText->setText(timeString);
	changeTextTimer->start(1);
}

void UachievementWidget::closeWidget() {
	removeFromViewport();
}

void UachievementWidget::updateEquipmentWidget() {
	equippedWidget->update();
	
}

void UachievementWidget::updateAchievementIcon(int id) {
	widget* child = achievementHolder->getChildAt(id);
	UachievementBox* box = dynamic_cast<UachievementBox*>(child);
	if (box)
		box->updateAchievementImage();
}