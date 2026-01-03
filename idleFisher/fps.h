#pragma once

#include <iostream>
#include "deferredPtr.h"

class Timer;

class fps {
public:
	fps();
	static void update(float deltaTime);
	static void returnTimer();
	static void showFPS(bool show);
	// displayes fps when fps is an outlier, call every frame
	static void outliers(float deltaTime);
	static void returnTimerOutlier();

	static inline DeferredPtr<Timer> fpsTimer;
	static inline DeferredPtr<Timer> outlierTimer;

	static inline bool bShow = false;
	static inline float total = 0;
	static inline float num = 0;

	static inline float totalOutlier = 0;
	static inline float numOutlier = 0;
};