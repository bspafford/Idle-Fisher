#pragma once

#include <string>
#include <unordered_map>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct GLFWcursor;

enum CursorType {
	CURSOR_DEFAULT,
	CURSOR_POINT,
	CURSOR_HOVER,
	CURSOR_GRAB,
	CURSOR_CLICK,
	CURSOR_HOOK
};

class Cursor {
public:
	static void calcMouseImg();

	static bool getMouseOverWater();
	static void setMouseOverWater(bool overWater);

	static void toggleCursor();

private:
	static void setCursorIcon(CursorType cursorName);

	static inline GLFWcursor* cursor;
	static inline CursorType currCursor;

	// if mouse is over water
	static inline bool mouseOverWater = false;

	static inline std::unordered_map<CursorType, GLuint> cursorLookup{
		{ CURSOR_DEFAULT,  GLFW_ARROW_CURSOR },
		{ CURSOR_POINT, GLFW_POINTING_HAND_CURSOR },
		{ CURSOR_HOVER, GLFW_POINTING_HAND_CURSOR },
		{ CURSOR_GRAB, GLFW_POINTING_HAND_CURSOR },
		{ CURSOR_CLICK, GLFW_POINTING_HAND_CURSOR },
	};
};