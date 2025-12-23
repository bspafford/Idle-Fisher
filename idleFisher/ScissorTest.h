#pragma once

#include <stack>
#include <glm/glm.hpp>

#include "math.h"

class ScissorTest {
public:
	static void Enable(vector loc, vector size, glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));
	static void Enable(Rect rect, glm::vec4 clearColor = glm::vec4(0, 0, 0, 0));
	static void Disable();

private:
	static inline std::stack<Rect> scissorStack;
};