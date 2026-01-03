#pragma once

#include "widget.h"
#include "deferredPtr.h"

class UprogressBar;
class Timer;

class UcomboOvertimeWidget : public widget {
public:
	UcomboOvertimeWidget(widget* parent);
	~UcomboOvertimeWidget();
	void draw(Shader* shaderProgram);

	template <class T> void addFinishedCallback(T* const object, void(T::* const update)()) {
		finishedCallback_ = std::bind_front(update, object);
	}

	// When player has put bobber back in water and the combo overtime is counting down
	void Start(float duration);
	void Refill();
private:
	void updateProgressBar();
	void setupLocs() override;
	void FinishedRefill();
	void OvertimeFinished();

	std::unique_ptr<UprogressBar> progressBar;
	DeferredPtr<Timer> comboTimer;
	DeferredPtr<Timer> refillTimer;

	bool refilling = false;
	// the ammount the percent that the previous timer left off at
	float refillAmount = 0.f;

	std::function<void()> finishedCallback_ = nullptr;
};
