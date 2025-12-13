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

// Compute an orthographic projection matrix that maps (0,0) at the top-left
// to (width, height) at the bottom-right. The matrix is in column-major order.
void math::computeOrthoMatrix(int width, int height, float* matrix) {
	// In our desired coordinate system:
	// left = 0, right = width, top = 0, bottom = height, near = -1, far = 1.
	float left = 0.0f;
	float right = static_cast<float>(width);
	float top = 0.0f;
	float bottom = static_cast<float>(height);
	float near = -1.0f;
	float far = 1.0f;

	// Compute orthographic projection (using column-major order).
	// Note: Because we want (0,0) at the top-left, we set top=0 and bottom=height.
	matrix[0] = 2.0f / (right - left);
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	// (top - bottom) is negative, which flips the y-axis.
	matrix[5] = 2.0f / (top - bottom);
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = -2.0f / (far - near);
	matrix[11] = 0.0f;

	//matrix[12] = -(left + right) / (left - right);
	matrix[12] = -(right + left) / (right - left);
	matrix[13] = -(top + bottom) / (top - bottom);
	matrix[14] = -(far + near) / (far - near);
	matrix[15] = 1.0f;
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
	// get local time and convert to string
	// auto const time = std::chrono::current_zone()
	//	->to_local(timePoint);

	std::string timestring = std::format("{:%Y-%m-%d %X}", timePoint);

	return timestring;
}

vector math::screenToWorld(vector mousePos, bool round) {
	vector playerPos = SaveData::saveData.playerLoc;
	playerPos.x *= -1;
	vector worldPos = (mousePos - playerPos * stuff::pixelSize) - stuff::screenSize / 2.f + vector{ 35, -81 }; // ??? idk why i gotta do the + {35, -81}, might be like half the character or something

	worldPos.y *= -1.f;
	return worldPos;
}

vector math::worldToScreen(vector pos, std::string origin) {
	vector center = screenToWorld({ 1920.f / 2.f, 1080.f / 2.f }) / stuff::pixelSize;
	vector screenCoords = (center - pos) * stuff::pixelSize;

	if (origin == "topleft")
		screenCoords = (screenCoords + stuff::screenSize / 2.f * vector{ -1.f, 1.f }) * vector{ -1.f, 1.f };
	return screenCoords;
	
}

glm::vec2 math::convertToRelativeCoords(glm::vec3 movement) {
	// Define the "right" vector (for horizontal movement).
	// This vector should be normalized.
	glm::vec3 right = glm::vec3(0.707f, 0.0f, -0.707f);

	// Define the "forward" vector (for vertical movement).
	// This vector should be normalized.
	glm::vec3 forward = glm::vec3(-0.707f, 0.0f, -0.707f);

	// Compute the relative coordinates by projecting 'movement' onto these axes.
	float relativeX = glm::dot(movement, right);
	float relativeY = glm::dot(movement, forward);

	return glm::vec2(relativeX, relativeY);
}

glm::vec3 math::convertFromRelativeCoords(glm::vec2 relative) {
	glm::vec3 right = glm::vec3(0.707f, 0.0f, -0.707f);
	glm::vec3 forward = glm::vec3(-0.707f, 0.0f, -0.707f);

	return relative.x * right + relative.y * forward;
}

std::string math::toLower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
		return std::tolower(c);
		});
	return str;
}