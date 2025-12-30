#pragma once

#define M_PI 3.1415926535f

#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <algorithm>
#include "vector.h"

class Image;

struct Rect {
	float x = 0;
	float y = 0;
	float w = 0;
	float h = 0;

	Rect() {}
	template<typename T>
	Rect(T _x, T _y, T _w, T _h) : x(static_cast<float>(_x)), y(static_cast<float>(_y)), w(static_cast<float>(_w)), h(static_cast<float>(_h)) {}

	friend std::ostream& operator<<(std::ostream& os, const Rect& r) {
		os << "(" << r.x << ", " << r.y << ", " << r.w << ", " << r.h << ")";
		return os;
	}

	Rect operator/ (vector v) const {
		return { x / v.x, y / v.y, w / v.x, h / v.y };
	}
};

class math {
public:
	static float dot(vector a, vector b);
	static vector normalize(vector a);
	static float length(vector value);
	static float distance(float x1, float y1, float x2, float y2);
	static float distance(vector a, vector b);
	static float min(float a, float b);
	static double min(double a, double b);
	static float max(float a, float b);
	static float max(double a, double b);
	static float lerp(float min, float max, float val);
	static int lerp(int min, int max, double val);
	static float randRange(float min, float max);
	static double randRange(double min, double max);
	static int randRangeInt(int min, int max);
	static float clamp(float x, float min, float max);
	static double clamp(double x, double min, double max);
	static vector perp(vector v);

	// quick so its approximation
	static float Q_sqrt(float number);
	// does the inverse (1/sqrt)
	static float Q_rsqrt(float number);

	// helper functions
	static vector screenToWorld(vector pos, bool round = true);

	static vector worldToScreen(vector pos);

	// time stuff
	// Y-M-D h:m:s
	static std::chrono::system_clock::time_point getTimeFromString(std::string timeString);
	static std::string getStringFromTime(std::chrono::system_clock::time_point timePoint);

	// lowercases string
	static std::string toLower(std::string str);

	// rect: location and size
	static bool IsPointInRect(vector p, Rect rect);
	static bool IsPointInRect(vector p, vector min, vector max);
};