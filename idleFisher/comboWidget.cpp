#include "comboWidget.h"

#include "main.h"
#include "math.h"
#include "stuff.h"
#include "shortNumbers.h"
#include "character.h"
#include "text.h"
#include "upgrades.h"

#include "debugger.h"

UcomboWidget::UcomboWidget(widget* parent) : widget(parent) {
	comboText = std::make_unique<text>(this, " ", "biggerStraight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	shake = std::make_unique<Eshake>(0);

	spawnComboNumber();
}

UcomboWidget::~UcomboWidget() {

}

void UcomboWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	vector shakeLoc = shake->getShakeLoc();
	comboText->setLoc(stuff::screenSize / (2.f * stuff::pixelSize) + vector(0.f, -36) + shakeLoc);
	comboText->draw(shaderProgram);
}

void UcomboWidget::update(float deltaTime) {

}

void UcomboWidget::spawnComboNumber() {
	double comboNum = GetCharacter()->GetCombo();
	comboText->setText("x" + shortNumbers::convert2Short(comboNum));

	if (comboNum == 1.0)
		hideComboText();
	else if (GetCharacter()->GetIsFishing())
		showComboText();

	prevComboNum = comboNum;

	shake->start({ 0, 0 });
	shake->setShakeDist(GetCharacter()->GetCombo() / Upgrades::Get(Stat::ComboMax) / 10.f);
}

void UcomboWidget::showComboText() {
	if (GetCharacter()->GetIsFishing())
		setVisibility(true);
}

void UcomboWidget::hideComboText() {
	visible = false;
}