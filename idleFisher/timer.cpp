#include "timer.h"

#include <iostream>

#include "main.h"
#include "math.h"

#include "debugger.h"

void Timer::GoingToDelete() {
	stop();
}

Timer::~Timer() {
	stop();
}

// calls update function to all instances of object
void Timer::callUpdate(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	// get value instead of reference so Timers / size aren't updating during the loop
	std::vector<Timer*> instances = DeferredPtr<Timer>::GetInstanceListVal();
	for (auto& instance : instances)
		instance->Update(deltaTime);

	DeferredPtr<Timer>::FlushDeferred();
}

void Timer::setFps(float fps) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
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
	std::lock_guard<std::recursive_mutex> lock(mutex);
	return time;
}

float Timer::getMaxTime() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	return maxTime;
}

float Timer::getPercent() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	return time / maxTime;
}

bool Timer::IsFinished() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	return bFinished;
}

bool Timer::IsGoing() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	return bGoing;
}

void Timer::start(float maxTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	bGoing = true;
	bFinished = false;

	time = 0;
	this->maxTime = maxTime;
}

void Timer::stop() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	bGoing = false;
	time = 0;
}