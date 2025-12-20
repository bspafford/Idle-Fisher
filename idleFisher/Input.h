#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

#include "math.h"
#include "Hoverable.h"

enum MouseButton {
	MOUSE_BUTTON_LEFT = GLFW_MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_BACKWARDS = GLFW_MOUSE_BUTTON_4,
	MOUSE_BUTTON_FORWARDS = GLFW_MOUSE_BUTTON_5
};

class Input {
public:
	static void Init();

	// callback functions
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// sets all Up and Down buttons to false, and calles glfwPollEvents()
	static void pollEvents();
	// calls the callback functions that were held to the end of the frame
	// example: leftClickCallback
	static void fireHeldInputs();

	// returns true if user pressess mouse button down that frame
	static bool getMouseButtonDown(int mouseButton);
	// returns true if user releases mouse button that frame
	static bool getMouseButtonUp(int mouseButton);
	// returns true for every frame the mouse button is held down for
	static bool getMouseButtonHeld(int mouseButton);
	// returns mouse scroll direction that frame
	// 1 = scroll up, -1 = scroll down
	static int getMouseScrollDir();
	static vector getMousePos();

	template <class T> static void setLeftClick(T* const object, void (T::* const callback) ()) {
		leftClickCallback = std::pair(dynamic_cast<IHoverable*>(object), std::bind_front(callback, object));
	}

	// returns true for the frame the key went down
	static bool getKeyDown(int key);
	// returns true for the frame they key went up
	static bool getKeyUp(int key);
	// returns true every frame the key is held down
	static bool getKeyHeld(int key);

private:
	// if mouse button was pressed that frame
	static inline bool mouseDown[5];
	// if mouse button released that frame
	static inline bool mouseUp[5];
	// returns true for every frame the mouse button is held down for
	static inline bool mouseHeld[5];
	static inline int mouseScrollDir = 0;
	static inline vector mousePos;

	// front most items that was clicked that frame
	static inline std::pair<IHoverable*, std::function<void()>> leftClickCallback;

	static inline bool keysDown[348];
	static inline bool keysUp[348];
	static inline bool keysHeld[348];
};