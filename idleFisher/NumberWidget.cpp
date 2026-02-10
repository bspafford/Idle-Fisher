#include "NumberWidget.h"
#include "text.h"
#include "timer.h"
#include "shortNumbers.h"

NumberData::NumberData(vector loc, double value, NumberType type, bool useWorldPos) {
	vector randOffset = vector(math::randRange(-7.f, 7.f), math::randRange(-5.f, 5.f));
	startLoc = loc + randOffset;

	std::string textString, font;
	glm::vec4 color;
	MakeNumberString(value, type, textString, font, color);
	number = std::make_unique<text>(nullptr, textString, font, startLoc, useWorldPos, false, TEXT_ALIGN_CENTER);
	number->setTextColor(color);

	// , NumberPrefix prefix, NumberSuffix suffix,

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

void NumberWidget::Start(vector loc, double value, NumberType type) {
	std::unique_ptr<NumberData> number = std::make_unique<NumberData>(loc, value, type, useWorldPos);
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

void NumberData::MakeNumberString(double value, NumberType type, std::string& textString, std::string& font, glm::vec4& color) {
	switch (type) {
	case NumberType::FishCaught:
		textString = "+" + shortNumbers::convert2Short(value);
		font = "straight";
		color = glm::vec4(56.f / 255.f, 1.f, 117.f / 255.f, 1.f); // green
		break;
	case NumberType::Recast:
		textString = "x" + shortNumbers::convert2Short(value + 1);
		font = "biggerStraight";
		color = glm::vec4(1, 0.5, 0, 1); // orange
		break;
	case NumberType::PremiumCash:
		textString = "+" + shortNumbers::convert2Short(value);
		font = "biggerStraight";
		color = glm::vec4(1, 1, 0, 1); // yellow
		break;
	case NumberType::PremiumBuff:
		textString = "x" + shortNumbers::convert2Short(value) + "%";
		font = "biggerStraight";
		color = glm::vec4(1, 1, 0, 1); // yellow
		break;
	case NumberType::Size:
		textString = "New Record: " + shortNumbers::convert2Short(value) + "in!";
		font = "straight";
		color = glm::vec4(0, 1, 1, 1); // cyan
	}

	if (value == 0)
		color = glm::vec4(0.75, 0.75, 0.75, 1);
}