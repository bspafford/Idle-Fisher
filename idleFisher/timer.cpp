#include "timer.h"
#include "math.h"

#include <iostream>

#include "debugger.h"

// calls update function to all instances of object
void Timer::callUpdate(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	// get value instead of reference so Timers / size aren't updating during the loop
	std::vector<Timer*> instances = DeferredPtr<Timer>::GetInstanceListVal();
	for (auto& instance : instances)
		instance->Update(deltaTime);

	DeferredPtr<Timer>::FlushDeferred();
}

void Timer::SetTime(float maxTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	this->maxTime = maxTime;
}

void Timer::Update(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	if (!bGoing)
		return;

	time += deltaTime;
	if (updateCallback_ && time <= maxTime)
		updateCallback_();

	if (time >= maxTime) {
		// call callback function
		if (loop) {
			time -= maxTime; // subtract time like normal
			float num = time / maxTime; // how many timers have passed since last frame
			float remainder = num - floor(num); // how much time the last time is in to its time
			for (int i = 0; i < floor(num); i++) {
				if (callback_)
					callback_();
			}

			time = remainder * maxTime;

			start(maxTime, loop);
		} else {
			stop();
			bFinished = true;
		}

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

void Timer::start(float maxTime, bool shouldLoop) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	bGoing = true;
	bFinished = false;
	loop = shouldLoop;

	this->maxTime = maxTime;
}