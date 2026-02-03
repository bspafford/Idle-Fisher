#pragma once

#include "widget.h"
#include "deferredPtr.h"

class text;
class Timer;

enum class NumberPrefix {
	None,
	Plus, // +
	Times, // x
};

enum class NumberSuffix {
	None,
	Percent, // %
};

enum class NumberType {
	FishCaught,
	Recast,
	PremiumCash, // if got the instant cash
	PremiumBuff, // if got the x10 or x100
};

struct NumberData {
	std::unique_ptr<text> number;
	DeferredPtr<Timer> timer;

	vector startLoc;
	float startRot;
	float randomTime;
	bool finished = false;

	NumberData(vector loc, double value, NumberType type, bool useWorldPos);

	void UpdateCallback();
	void Finished();

	void MakeNumberString(double value, NumberType type, std::string& text, std::string& font, glm::vec4& color);
};

class NumberWidget : public widget {
public:
	NumberWidget(widget* parent, bool useWorldPos);
	void Start(vector loc, double value, NumberType type);
	void draw(Shader* shader) override;

private:
	bool useWorldPos = false;
	std::vector<std::unique_ptr<NumberData>> numbers;
};