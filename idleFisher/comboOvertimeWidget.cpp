#include "comboOvertimeWidget.h"
#include "main.h"
#include "progressBar.h"
#include "timer.h"

#include "debugger.h"

UcomboOvertimeWidget::UcomboOvertimeWidget(widget* parent) : widget(parent) {
	progressBar = std::make_unique<UprogressBar>(this, vector{ 45.f, 3.f }, false);
	progressBar->SetAnchor(ANCHOR_CENTER, ANCHOR_TOP);
	progressBar->SetPivot({ 0.5f, 0.f });
	progressBar->setPercent(1.f);
	setVisibility(false);

	comboTimer = std::make_unique<timer>();
	comboTimer->addUpdateCallback(this, &UcomboOvertimeWidget::updateProgressBar);
	comboTimer->addFinishedCallback(this, &UcomboOvertimeWidget::OvertimeFinished);

	refillTimer = std::make_unique<timer>();
	refillTimer->addUpdateCallback(this, &UcomboOvertimeWidget::updateProgressBar);
	refillTimer->addFinishedCallback(this, &UcomboOvertimeWidget::FinishedRefill);

	setupLocs();

}

UcomboOvertimeWidget::~UcomboOvertimeWidget() {

}

void UcomboOvertimeWidget::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	progressBar->draw(shaderProgram);
}

void UcomboOvertimeWidget::Start(float duration) {
	refilling = false;

	setVisibility(true);

	float timeRemaining = progressBar->getPercent() * duration;
	comboTimer->start(timeRemaining);
	refillAmount = progressBar->getPercent();

	refillTimer->stop();
}

void UcomboOvertimeWidget::Refill() {
	if (comboTimer->IsGoing()) {
		refilling = true;
		
		float timeRemaining = (1.f - progressBar->getPercent()) * comboTimer->getMaxTime();
		refillTimer->start(timeRemaining);

		refillAmount = progressBar->getPercent();

		comboTimer->stop();
	}
}

void UcomboOvertimeWidget::updateProgressBar() {
	if (refilling) {
		float scaledPercent = math::lerp(refillAmount, 1.f, refillTimer->getPercent());
		progressBar->setPercent(scaledPercent);
	} else {
		float scaledPercent = math::lerp(0.f, refillAmount, 1.f - comboTimer->getPercent());
		progressBar->setPercent(scaledPercent);
	}
}

void UcomboOvertimeWidget::setupLocs() {
	progressBar->setLoc(vector{ 0.f, -16.f });
}

void UcomboOvertimeWidget::FinishedRefill() {
	refilling = false;
	setVisibility(false);
}

void UcomboOvertimeWidget::OvertimeFinished() {
	if (finishedCallback_)
		finishedCallback_();
}