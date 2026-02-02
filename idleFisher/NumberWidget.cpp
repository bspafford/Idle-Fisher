#include "NumberWidget.h"
#include "text.h"
#include "timer.h"
#include "shortNumbers.h"

NumberData::NumberData(vector loc, double value, NumberPrefix prefix, NumberSuffix suffix, bool useWorldPos) {
	vector randOffset = vector(math::randRange(-5.f, 5.f), math::randRange(-3.f, 4.f));
	startLoc = loc + randOffset;

	std::string textString = CalcPrefix(prefix) + shortNumbers::convert2Short(value) + CalcSuffix(suffix);
	number = std::make_unique<text>(nullptr, textString, "straight", startLoc, useWorldPos, false, TEXT_ALIGN_CENTER);

	timer = CreateDeferred<Timer>();
	timer->addUpdateCallback(this, &NumberData::UpdateCallback);
	timer->addCallback(this, &NumberData::Finished);

	startRot = math::randRange(-20.f, 20.f);
	randomTime = math::randRange(0.75f, 1.25f);

	timer->start(randomTime);
}

void NumberData::Finished() {
	finished = true;
}

void NumberData::UpdateCallback() {
	float percent = timer->getPercent();
	float easeInCubic = percent * percent * percent;

	number->SetAlpha(1.f - easeInCubic);
	number->setLoc(startLoc + vector(0.f, easeInCubic * 10.f));
}

NumberWidget::NumberWidget(widget* parent, bool useWorldPos) : widget(parent) {
	this->useWorldPos = useWorldPos;
}

void NumberWidget::Start(vector loc, double value, NumberPrefix prefix, NumberSuffix suffix) {
	std::unique_ptr<NumberData> number = std::make_unique<NumberData>(loc, value, prefix, suffix, useWorldPos);
	numbers.push_back(std::move(number));
}

void NumberWidget::draw(Shader* shader) {
	widget::draw(shader);

	for (int i = 0; i < numbers.size(); i++) {
		auto& number = numbers[i];
		if (number->finished) { // delete finished
			numbers.erase(numbers.begin() + i);
			--i;
		} else
			number->number->draw(shader);
	}
}

std::string NumberData::CalcPrefix(NumberPrefix prefix) {
	switch (prefix) {
	case NumberPrefix::Plus:
		return "+";
	case NumberPrefix::Times:
		return "x";
	default:
		return "";
	}
}

std::string NumberData::CalcSuffix(NumberSuffix suffix) {
	switch (suffix) {
	case NumberSuffix::Percent:
		return "%";
	default:
		return "";
	}
}