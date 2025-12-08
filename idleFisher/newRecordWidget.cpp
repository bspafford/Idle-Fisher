#include "newRecordWidget.h"
#include "text.h"
#include "timer.h"

#include "debugger.h"

UnewRecordWidget::UnewRecordWidget(widget* parent) : widget(parent) {
	recordText = std::make_unique<text>(this, "New Record!", "straight", vector{ 0, 0 }, false, false, TEXT_ALIGN_CENTER);
	startTimer = std::make_unique<timer>();
	startTimer->addCallback(this, &UnewRecordWidget::finished);
	setVisibility(false);
}

UnewRecordWidget::~UnewRecordWidget() {

}

void UnewRecordWidget::draw(Shader* shaderProgram) {
	__super::draw(shaderProgram);

	if (!visible)
		return;

	if (recordText) {
		recordText->setLoc(stuff::screenSize / 2.f - vector{ 0, 40 * stuff::pixelSize });
		recordText->draw(shaderProgram);
	}
}

void UnewRecordWidget::start(int fishSize) {
	recordText->setText("New Record: " + std::to_string(fishSize) + "in!");
	setVisibility(true);
	startTimer->start(3);
}

void UnewRecordWidget::finished() {
	setVisibility(false);
}