#include "comboWidget.h"

#include "main.h"
#include "math.h"
#include "stuff.h"
#include "shortNumbers.h"
#include "character.h"
#include "text.h"

#include "debugger.h"

UcomboWidget::UcomboWidget(widget* parent) : widget(parent) {
	comboText = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);

	spawnComboNumber(1);
}

UcomboWidget::~UcomboWidget() {

}

void UcomboWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	comboText->draw(shaderProgram);
}

void UcomboWidget::update(float deltaTime) {

}

void UcomboWidget::spawnComboNumber(double comboNum) {
	comboText->setText("x" + shortNumbers::convert2Short(comboNum));
	setupLocs();

	if (comboNum == 1.0)
		hideComboText();
	else if (Main::character->isFishing)
		showComboText();

	prevComboNum = comboNum;
}

void UcomboWidget::setupLocs() {
	comboText->setLoc({ stuff::screenSize.x / 2.f, 4 * stuff::pixelSize });
}

void UcomboWidget::showComboText() {
	if (Main::character->isFishing)
		visible = true;
}

void UcomboWidget::hideComboText() {
	visible = false;
}