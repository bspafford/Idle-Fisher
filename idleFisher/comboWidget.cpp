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
	comboText->SetPivot({ 0.f, 1.f });

	spawnComboNumber();
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

void UcomboWidget::spawnComboNumber() {
	double comboNum = GetCharacter()->GetCombo();
	comboText->setText("x" + shortNumbers::convert2Short(comboNum));
	setupLocs();

	if (comboNum == 1.0)
		hideComboText();
	else if (GetCharacter()->isFishing)
		showComboText();

	prevComboNum = comboNum;
}

void UcomboWidget::setupLocs() {
	comboText->setLoc({ stuff::screenSize.x / (stuff::pixelSize * 2.f), stuff::screenSize.y / stuff::pixelSize - 4.f });
}

void UcomboWidget::showComboText() {
	if (GetCharacter()->isFishing)
		setVisibility(true);
}

void UcomboWidget::hideComboText() {
	visible = false;
}