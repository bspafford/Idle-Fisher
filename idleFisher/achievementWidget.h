#pragma once

#include "widget.h"
#include "timer.h"

class text;
class Ubutton;
class UhoverBox;
class UscrollBox;
class UwrapBox;
class UequippedWidget;
class UachievementBox;

class UachievementWidget : public widget {
public:
	UachievementWidget(widget* parent);
	~UachievementWidget();

	void draw(Shader* shaderProgram) override;

	void updateEquipmentWidget();
	void updateAchievementIcon(int id);
private:
	void setupLocs() override;
	void closeWidget();
	void updateText();
	void addedToViewport() override;

	std::unique_ptr<Image> background;

	DeferredPtr<Timer> changeTextTimer;

	std::unique_ptr<text> startTimeText;
	std::unique_ptr<UequippedWidget> equippedWidget;
	std::unique_ptr<UscrollBox> scrollBox;
	std::unique_ptr<text> achievementText;
	std::unique_ptr<UwrapBox> achievementHolder;
	std::unique_ptr<UhoverBox> hoverBox;
	std::unique_ptr<Ubutton> xButton;
	std::vector<std::unique_ptr<UachievementBox>> achievementBoxList;
};