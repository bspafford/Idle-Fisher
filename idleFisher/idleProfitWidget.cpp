#include "idleProfitWidget.h"
#include "main.h"
#include "button.h"
#include "text.h"
#include "heldFishWidget.h"
#include "scrollBox.h"
#include "Rectangle.h"

#include "debugger.h"

UidleProfitWidget::UidleProfitWidget(widget* parent) : widget(parent) {
	collectButton = std::make_unique<Ubutton>(this, "widget/button.png", 54, 39, 2, vector{ 0, 0 }, false, false);

	title = std::make_unique<text>(this, "Idle Profit", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	std::vector<std::string> collectButtonAnim = { "./images/widget/button1.png", "./images/widget/button2.png" };
	//collectButton->setClickAnim(collectButtonAnim);
	collectButton->addCallback<widget>(this, &widget::removeFromViewport);

	collectText = std::make_unique<text>(this, "Collect", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	scrollBox = std::make_unique<UscrollBox>(this);
	heldFishWidget = std::make_unique<UheldFishWidget>(this);

	setupLocs();
}

UidleProfitWidget::~UidleProfitWidget() {

}

void UidleProfitWidget::draw(Shader* shaderProgram) {
	if (background)
		background->draw(shaderProgram);

	title->draw(shaderProgram);
	collectButton->draw(shaderProgram);
	collectText->draw(shaderProgram);

	return;
	scrollBox->draw(shaderProgram);
}

void UidleProfitWidget::setup(std::vector<FsaveFishData> fishList) {
	heldFishWidget->updateList(fishList);
	scrollBox->addChild(heldFishWidget.get(), heldFishWidget->getSize().y);
	scrollBox->setLocAndSize({ .35f * stuff::screenSize.x, .1f * stuff::screenSize.y }, { 40 * stuff::pixelSize, .8f * stuff::screenSize.y });
	setupLocs();
}

void UidleProfitWidget::addedToViewport() {

}

void UidleProfitWidget::setupLocs() {
	background = std::make_unique<URectangle>(vector{ .35f * stuff::screenSize.x, .1f * stuff::screenSize.y }, vector{ .3f * stuff::screenSize.x, .8f * stuff::screenSize.y }, false);
	background->setColor(glm::vec4(glm::vec3(0.f), 1.f));
	//background = { int(.35 * stuff::screenSize.x), int(.1 * stuff::screenSize.y), int(.3 * stuff::screenSize.x), int(.8 * stuff::screenSize.y)};

	title->setLoc(vector{ stuff::screenSize.x / 2.f, background->getLoc().y + 5 * stuff::pixelSize});

	vector buttonSize = collectButton->getSize();
	collectButton->setLoc(vector{ stuff::screenSize.x / 2.f, .9f * stuff::screenSize.y } - vector{buttonSize.x / 2, buttonSize.y } - vector{ 0, 2 * stuff::pixelSize });
	//collectText->setLoc(vector{ stuff::screenSize.x / 2.f, .9f * stuff::screenSize.y } - vector{ 0, collectText->getSize().y } - vector{ 0, 2 * stuff::pixelSize });
	collectText->setLoc(collectButton->getLoc() + buttonSize / 2 - vector{0, collectText->getSize().y / 2}); // half button - half text.y

	scrollBox->setLoc(vector{stuff::screenSize.x / 2.f, stuff::screenSize.y / 2.f} - heldFishWidget->getSize() / 2);
}
