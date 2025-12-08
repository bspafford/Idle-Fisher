#include "confirmWidget.h"

ConfirmWidget::ConfirmWidget(widget* parent) : widget(parent) {
	backgroundImage = std::make_unique<Image>("./images/widget/confirmBox.png", vector{ 0, 0 }, false);
	saveText = std::make_unique<text>(this, "Do you want to save settings?", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	saveButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	revertButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	cancelButton = std::make_unique<Ubutton>(this, "widget/upgradeButton.png", 37, 16, 1, vector{ 0, 0 }, false, false);
	saveButtonText = std::make_unique<text>(this, "Save", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	revertButtonText = std::make_unique<text>(this, "Revert", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	cancelButtonText = std::make_unique<text>(this, "Cancel", "straightDark", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	rect = std::make_unique<URectangle>(vector{ 0, 0 }, stuff::screenSize, false, glm::vec4(0, 0, 0, 0.333f));
	rect->setBlockCursor(true);

	setLoc(loc);
}

void ConfirmWidget::draw(Shader* shader) {
	rect->setSize(stuff::screenSize);
	rect->draw(shader);

	backgroundImage->draw(shader);
	saveText->draw(shader);
	saveButton->draw(shader);
	revertButton->draw(shader);
	cancelButton->draw(shader);
	saveButtonText->draw(shader);
	revertButtonText->draw(shader);
	cancelButtonText->draw(shader);
}

void ConfirmWidget::setLoc(vector loc) {
	vector center = stuff::screenSize / 2.f;
	vector backgroundLoc = center - backgroundImage->getSize() / 2.f;
	backgroundImage->setLoc(backgroundLoc);
	saveText->setLoc(center + vector(0, -8) * stuff::pixelSize);
	saveButton->setLoc(backgroundLoc + vector(7, 55) * stuff::pixelSize);
	revertButton->setLoc(backgroundLoc + vector(47, 55) * stuff::pixelSize);
	cancelButton->setLoc(backgroundLoc + vector(87, 55) * stuff::pixelSize);
	saveButtonText->setLoc(saveButton->getLoc() + saveButton->getSize() / 2.f);
	revertButtonText->setLoc(revertButton->getLoc() + revertButton->getSize() / 2.f);
	cancelButtonText->setLoc(cancelButton->getLoc() + cancelButton->getSize() / 2.f);
}

void ConfirmWidget::setupLocs() {
	setLoc(loc);
}