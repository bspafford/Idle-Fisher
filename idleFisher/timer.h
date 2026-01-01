#pragma once

#include <set>
#include <iostream>
#include <cassert>
#include <functional>
#include <mutex>

class timer {
public:
	timer();
	~timer();

	static void callUpdate(float deltaTime);

	static void clearInstanceList(bool changingWorlds);

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
	template <class T> void addCallback(T* const object, void(T::* const finish)(), void(T::* const update)() = NULL) {
		callback_ = std::bind_front(finish, object);
		if (update != NULL)
			updateCallback_ = std::bind_front(update, object);
	}

	// works for static functions
	void addCallback(void (*callback) ()) {
		callback_ = callback;
	}

	template <class T> void addUpdateCallback(T* const object, void(T::* const update)()) {
		updateCallback_ = std::bind_front(update, object);
	}

	void addUpdateCallback(void (*callback) ()) {
		updateCallback_ = callback;
	}

	template <class T> void addFinishedCallback(T* const object, void(T::* const update)()) {
		finishedCallback_ = std::bind_front(update, object);
	}

	void addFinishedCallback(void (*callback) ()) {
		finishedCallback_ = callback;
	}

private:
	// recursive incase Update() or callbacks create a timer and lock mutex while its already locked
	static inline std::recursive_mutex mutex;

	// keeps track of all instances to call update function on
	static inline std::vector<timer*> instances;

	float time = 0;
	float maxTime;
	bool bGoing = false;
	bool bFinished = false;

	std::function<void()> callback_ = nullptr;
	std::function<void()> updateCallback_ = nullptr;
	std::function<void()> finishedCallback_ = nullptr;

	// whether or not this object should be removed from instance list when changing worlds
	bool dontDelete;
};