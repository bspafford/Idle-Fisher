#include "timer.h"

#include <iostream>

#include "main.h"
#include "math.h"

#include "debugger.h"

Timer::Timer() {
	std::lock_guard<std::recursive_mutex> lock(mutex);

}

Timer::~Timer() {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	stop();
}

// calls update function to all instances of object
void Timer::callUpdate(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	for (auto& instance : DeferredPtr<Timer>::GetInstanceList())
		instance->Update(deltaTime);

	DeferredPtr<Timer>::FlushDeferred();
}

void Timer::setFps(float fps) {
	maxTime = fps;
}

void Timer::Update(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	if (!bGoing)
		return;

	time += deltaTime;
	time = math::clamp(time, 0, maxTime);
	if (updateCallback_ && time <= maxTime)
		updateCallback_();

	if (time >= maxTime) {
		// call callback function
		stop();
		bFinished = true;
		if (callback_)
			callback_();
	}
}

float Timer::getTime() {
	return time;
}

float Timer::getMaxTime() {
	return maxTime;
}

float Timer::getPercent() {
	return time / maxTime;
}

bool Timer::IsFinished() {
	return bFinished;
}

bool Timer::IsGoing() {
	return bGoing;
}

void Timer::start(float maxTime) {
	bGoing = true;
	bFinished = false;

	time = 0;
	this->maxTime = maxTime;
}

void Timer::stop() {
	bGoing = false;
	time = 0;
}

void Timer::shouldntDelete(bool dontDelete) {
	this->dontDelete = dontDelete;
}