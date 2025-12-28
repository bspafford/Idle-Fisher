#pragma once

#include <stack>
#include <glm/glm.hpp>

#include "math.h"

class ScissorTest {
public:
	static void Enable(vector loc, vector size);
	static void Enable(Rect rect);
	static void Disable();
	static Rect GetCurrRect();
private:
	static inline std::stack<Rect> scissorStack;
};