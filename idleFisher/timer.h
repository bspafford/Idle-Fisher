#pragma once

#include <set>
#include <iostream>
#include <cassert>
#include <functional>
#include <mutex>

#include "deferredPtr.h"

class Timer {
private:
	Timer();
	friend class CreateDeferred<Timer>; // forces Timer to be created by deferredPtr cause Timer constructor is private

public:
	~Timer();

	static void callUpdate(float deltaTime);

	void Update(float deltaTime);

	void setFps(float fps);

	// this starts the timer
	void start(float maxTime);
	void stop();

	float getTime();
	float getMaxTime();
	float getPercent();

	bool IsFinished();
	bool IsGoing();

	void shouldntDelete(bool dontDelete);

	// sets up callback and fps
	template <typename T> void addCallback(T* const object, void(T::* const finish)()) {
		callback_ = std::bind_front(finish, object);
	}

	// works for static functions
	void addCallback(void (*callback) ()) {
		callback_ = callback;
	}

	template <typename T> void addUpdateCallback(T* const object, void(T::* const update)()) {
		updateCallback_ = std::bind_front(update, object);
	}

	void addUpdateCallback(void (*callback) ()) {
		updateCallback_ = callback;
	}

private:
	// recursive incase Update() or callbacks create a timer and lock mutex while its already locked
	static inline std::recursive_mutex mutex;

	float time = 0;
	float maxTime;
	bool bGoing = false;
	bool bFinished = false;

	std::function<void()> callback_ = nullptr;
	std::function<void()> updateCallback_ = nullptr;

	// whether or not this object should be removed from instance list when changing worlds
	bool dontDelete;
};