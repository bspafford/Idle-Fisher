#include "timer.h"

#include <iostream>

#include "main.h"
#include "math.h"

#include "debugger.h"

timer::timer() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	instances.push_back(this);
}

timer::~timer() {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	auto it = std::find(instances.begin(), instances.end(), this);
	if (it != instances.end())
		instances.erase(it);
}

// calls update function to all instances of object
void timer::callUpdate(float deltaTime) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	for (int i = 0; i < instances.size(); i++) {
		instances[i]->Update(deltaTime);
	}
}

void timer::clearInstanceList(bool changingWorlds) {
	std::lock_guard<std::recursive_mutex> lock(mutex);
	if (changingWorlds) {
		instances.erase(
			std::remove_if(instances.begin(), instances.end(),
				[](auto& timer) { return !timer->dontDelete; }
			),
			instances.end()
		);
	} else {
		instances.clear();
	}
}

void timer::setFps(float fps) {
	maxTime = fps;
}

void timer::Update(float deltaTime) {
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
		if (finishedCallback_)
			finishedCallback_();
	}
}

float timer::getTime() {
	return time;
}

float timer::getMaxTime() {
	return maxTime;
}

float timer::getPercent() {
	return time / maxTime;
}

bool timer::IsFinished() {
	return bFinished;
}

bool timer::IsGoing() {
	return bGoing;
}

void timer::start(float maxTime) {
	bGoing = true;
	bFinished = false;

	time = 0;
	this->maxTime = maxTime;
}

void timer::stop() {
	bGoing = false;
	time = 0;
}

void timer::shouldntDelete(bool dontDelete) {
	this->dontDelete = dontDelete;
}