#include "idleProfitWidget.h"
#include "main.h"
#include "button.h"
#include "text.h"
#include "heldFishWidget.h"
#include "scrollBox.h"
#include "Rectangle.h"

#include "debugger.h"

UidleProfitWidget::UidleProfitWidget(widget* parent) : widget(parent) {
	background = std::make_unique<Image>("./images/widget/idleProfitBackground.png", vector{ 0.f, 0.f }, false);
	background->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	background->SetPivot({ 0.5f, 0.5f });

	title = std::make_unique<text>(this, "Idle Profit", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	title->SetPivot({ 0.f, 1.f });

	collectButton = std::make_unique<Ubutton>(this, "widget/biggerButton.png", 31, 15, 1, vector{ 0, 0 }, false, false);
	collectButton->SetPivot({ 0.5f, 0.f });
	collectButton->addCallback<widget>(this, &widget::removeFromViewport);

	collectText = std::make_unique<text>(this, "Collect", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	collectText->SetPivot({ 0.f, 0.5f });

	heldFishWidget = std::make_unique<UheldFishWidget>(this);
	heldFishWidget->SetAnchor(ANCHOR_CENTER, ANCHOR_CENTER);
	heldFishWidget->SetPivot({ 0.5f, 0.5f });

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

	heldFishWidget->draw(shaderProgram);
}

void UidleProfitWidget::setup(std::vector<FsaveFishData> fishList) {
	heldFishWidget->updateList(fishList);
}

void UidleProfitWidget::setupLocs() {
	title->setLoc(background->getAbsoluteLoc() + background->getSize() / vector{ 2.f, 1.f } - vector{ 0.f, 8.f });

	collectButton->setLoc(background->getAbsoluteLoc() + vector{ background->getSize().x / 2.f, 8.f });
	collectText->setLoc(collectButton->getAbsoluteLoc() + collectButton->getSize() / 2.f);
}
