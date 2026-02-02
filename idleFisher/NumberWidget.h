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

struct NumberData {
	std::unique_ptr<text> number;
	DeferredPtr<Timer> timer;

	vector startLoc;
	float startRot;
	float randomTime;
	bool finished = false;

	NumberData(vector loc, double value, NumberPrefix prefix, NumberSuffix suffix, bool useWorldPos);

	void UpdateCallback();
	void Finished();

	std::string CalcPrefix(NumberPrefix prefix);
	std::string CalcSuffix(NumberSuffix suffix);
};

class NumberWidget : public widget {
public:
	NumberWidget(widget* parent, bool useWorldPos);
	void Start(vector loc, double value, NumberPrefix prefix, NumberSuffix suffix = NumberSuffix::None);
	void draw(Shader* shader) override;

private:
	bool useWorldPos = false;
	std::vector<std::unique_ptr<NumberData>> numbers;
};