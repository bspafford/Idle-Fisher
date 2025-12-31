#include "comboOvertimeWidget.h"
#include "main.h"
#include "progressBar.h"
#include "timer.h"

#include "debugger.h"

UcomboOvertimeWidget::UcomboOvertimeWidget(widget* parent) : widget(parent) {
	progressBar = std::make_unique<UprogressBar>(this, vector{ 45.f, 3.f }, false, FprogressBarDir::right, true);
	progressBar->SetAnchor(ANCHOR_CENTER, ANCHOR_TOP);
	progressBar->SetPivot({ 0.5f, 0.f });
	setVisibility(false);

	setupLocs();
}

UcomboOvertimeWidget::~UcomboOvertimeWidget() {

}

void UcomboOvertimeWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	progressBar->draw(shaderProgram);
}

void UcomboOvertimeWidget::addTimer(timer* timer) {
	comboTimer = timer;
	comboTimer->addUpdateCallback(this, &UcomboOvertimeWidget::updateProgressBar);
}

void UcomboOvertimeWidget::updateProgressBar() {
	progressBar->setPercent(comboTimer->getTime() / comboTimer->getMaxTime());
}

void UcomboOvertimeWidget::setupLocs() {
	progressBar->setLoc(vector{ 0.f, -16.f });
}