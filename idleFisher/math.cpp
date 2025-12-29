#include "math.h"
#include "main.h"
#include "camera.h"
#include "saveData.h"

#include "debugger.h"

// returns the dot product of two 2d vectors
float math::dot(vector a, vector b) {
	return a.x * b.x + a.y * b.y;
}

// normalizes the vector
vector math::normalize(vector a) {
	float invLen = Q_rsqrt(a.x * a.x + a.y * a.y);

	if (a.x == 0 && a.y == 0)
		return { 0, 0 };

	return { a.x * invLen, a.y * invLen };

}

float math::length(vector value) {
	return Q_sqrt(value.x * value.x + value.y * value.y);
}


// returns the distance between 2 points
float math::distance(float x1, float y1, float x2, float y2) {
	return Q_sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

}

float math::distance(vector a, vector b) {
	return Q_sqrt(((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)));
}

float math::min(float a, float b) {
	if (a < b) return a;
	return b;
}

double math::min(double a, double b) {
	if (a < b) return a;
	return b;
}

float math::max(float a, float b) {
	if (a > b) return a;
	return b;
}

float math::max(double a, double b) {
	if (a > b) return a;
	return b;
}

float math::lerp(float min, float max, float val) {
	float dif = max - min;
	return val * dif + min;
}

int math::lerp(int min, int max, double val) {
	float dif = max - min;
	return static_cast<int>(val * dif + min);
}

float math::randRange(float min, float max) {
	return (float)rand() / RAND_MAX * (max - min) + min;
}

double math::randRange(double min, double max) {
	return (double)rand() / RAND_MAX * (max - min) + min;
}

int math::randRangeInt(int min, int max) {
	return rand() % (max - min + 1) + min;
}

float math::clamp(float x, float min, float max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

double math::clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

vector math::perp(vector v) {
	return { -v.y, v.x };
}

float math::Q_sqrt(float number) {
	return Q_rsqrt(number) * number;
}

float math::Q_rsqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long*)&y;

	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y * (threehalfs - (x2 * y * y));
	y = y * (threehalfs - (x2 * y * y)); // adds extra accuracy

	return y;
}

std::chrono::system_clock::time_point math::getTimeFromString(std::string timeString) {
	// from string, back to time
	std::tm tm = {};
	std::istringstream ss(timeString);
	ss >> std::get_time(&tm, "%Y-%m-%d %X");

	time_t t = std::mktime(&tm) - _timezone;
	return std::chrono::system_clock::from_time_t(t);
}

std::string math::getStringFromTime(std::chrono::system_clock::time_point timePoint) {
	std::string timestring = std::format("{:%Y-%m-%d %X}", timePoint);
	return timestring;
}

vector math::screenToWorld(vector mousePos, bool round) {
	return SaveData::saveData.playerLoc - stuff::screenSize / (stuff::pixelSize * 2.f) + mousePos / stuff::pixelSize;
}

vector math::worldToScreen(vector pos) {
	return (pos - SaveData::saveData.playerLoc + stuff::screenSize / (stuff::pixelSize * 2.f));
}

std::string math::toLower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	return str;
}