#include "progressBar.h"

#include "debugger.h"

UprogressBar::UprogressBar(widget* parent, vector size, bool useWorldLoc, int dir, bool reverse) : widget(parent) {
	this->size = size;
	this->useWorldLoc = useWorldLoc;
	this->dir = dir;
	this->reverse = reverse;

	foregrounds.push_back(std::make_unique<URectangle>(absoluteLoc, size, useWorldLoc, glm::vec4(1.f)));
	background = std::make_unique<URectangle>(absoluteLoc, size, useWorldLoc, glm::vec4(1.f));
}

void UprogressBar::draw(Shader* shaderProgram) {
	if (!visible)
		return;

	background->draw(shaderProgram);
	vector offset = absoluteLoc;

	float totalWeight = calcTotalWeight();
	for (int i = 0; i < percent.size(); i++) {
		float percentVal = reverse ? 1 - percent[i] : percent[i];
		if (dir == FprogressBarDir::left) {
			float scaledSize = round(size.x * percentVal);
			foregrounds[0]->setLoc({ offset.x + size.x - scaledSize, offset.y });
			foregrounds[0]->setSize({ scaledSize, size.y });
		} else if (dir == FprogressBarDir::right) {
			foregrounds[i]->setLoc(offset);
			foregrounds[i]->setSize({ size.x * percentVal * (weight[i] / totalWeight), size.y });
			offset.x += size.x * percentVal * (weight[i] / totalWeight);

		} else if (dir == FprogressBarDir::up) {
			float scaledSize = round(size.y * percentVal);
			foregrounds[0]->setLoc({ offset.x, offset.y + size.y - scaledSize });
			foregrounds[0]->setLoc({ size.x, scaledSize });
		} else if (dir == FprogressBarDir::down) {
			foregrounds[0]->setLoc(offset);
			foregrounds[0]->setSize({ size.x, size.y * percentVal });
		}

		foregrounds[i]->draw(shaderProgram);
	}
}

void UprogressBar::setBackgroundColor(glm::vec4 color) {
	background->setColor(color);
}

void UprogressBar::setForegroundColor(glm::vec4 color, int id) {
	foregrounds[id]->setColor(color);
}

void UprogressBar::setPercent(float percent, int id) {
	this->percent[id] = math::clamp(percent, 0, 1);
}

void UprogressBar::addProgressBar(float percent, float weight, glm::vec4 color) {
	this->percent.push_back(percent);
	this->weight.push_back(weight);

	foregrounds.resize(this->percent.size());
	for (auto &foreground : foregrounds)
		if (!foreground)
			foreground = std::make_unique<URectangle>(absoluteLoc, size, useWorldLoc);
	foregrounds[foregrounds.size() - 1]->setColor(color);
}

float UprogressBar::calcTotalWeight() {
	float total = 0;
	for (float w : weight)
		total += w;
	return total;
}

void UprogressBar::setLoc(vector loc) {
	__super::setLoc(loc);

	background->setLoc(loc);
}

void UprogressBar::setSize(vector size) {
	__super::setSize(size);

	background->setSize(size);
}