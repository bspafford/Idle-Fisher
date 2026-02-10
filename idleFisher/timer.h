#pragma once

#include <set>
#include <iostream>
#include <cassert>
#include <functional>
#include <mutex>

#include "deferredPtr.h"

class Timer {
private:
	Timer() {}
	void GoingToDelete() { stop(); }
	friend class CreateDeferred<Timer>; // forces Timer to be created by deferredPtr because Timer constructor is private
	friend class DeferredPtr<Timer>;

public:
	~Timer() { stop(); }

	static void callUpdate(float deltaTime);

	void Update(float deltaTime);

	// how long it takes for timer to finish in seconds
	void SetTime(float maxTime);

	// this starts the timer
	void start(float maxTime, bool shouldLoop = false);
	void stop() {
		std::lock_guard<std::recursive_mutex> lock(mutex);
		bGoing = false;
		time = 0;
	}

	float getTime();
	float getMaxTime();
	float getPercent();

	bool IsFinished();
	bool IsGoing();

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
	std::recursive_mutex mutex;
	static inline std::mutex staticMutex;

	float time = 0;
	float maxTime;
	bool bGoing = false;
	bool bFinished = false;
	bool loop = false;

	std::function<void()> callback_ = nullptr;
	std::function<void()> updateCallback_ = nullptr;
};