#include "main.h"
#include "Input.h"
#include "widget.h"
#include "Hoverable.h"

#include "debugger.h"

void Input::Init() {
	// initialize cursor pos, otherwise will be (0, 0) until cursor is moved
	double mouseX, mouseY;
	glfwGetCursorPos(Main::GetWindow(), &mouseX, &mouseY);
	mousePos = vector{ static_cast<float>(mouseX), stuff::screenSize.y - static_cast<float>(mouseY) };
}

void Input::pollEvents() {
	// set mouse up and down to false
	for (int i = 0; i < 5; i++) {
		mouseDown[i] = false;
		mouseUp[i] = false;
	}
	mouseScrollDir = 0;

	leftClickCallback = std::pair(nullptr, nullptr);

	for (int i = 0; i < 348; i++) {
		keysDown[i] = false;
		keysUp[i] = false;
	}

	glfwPollEvents();
}

void Input::fireHeldInputs() {
	// make sure its the same item thats being hovered and clicked
	// makes sure that hovered item is the same one thats being clicked
	if (IHoverable::checkValidInteract() && leftClickCallback.second && leftClickCallback.first == IHoverable::getHoveredItem())
		leftClickCallback.second();
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		mouseDown[button] = true;
		mouseHeld[button] = true;
	} else if (action == GLFW_RELEASE) {
		mouseUp[button] = true;
		mouseHeld[button] = false;
	}
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	mouseScrollDir = yoffset;
}

void Input::cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {
	mousePos = vector{ static_cast<float>(xPos), stuff::screenSize.y - static_cast<float>(yPos) };
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == -1)
		return;

	if (action == GLFW_PRESS) {
		keysDown[key] = true;
		keysHeld[key] = true;
	} else if (action == GLFW_RELEASE) {
		keysUp[key] = true;
		keysHeld[key] = false;
	}
}

bool Input::getMouseButtonDown(int mouseButton) {
	return mouseDown[mouseButton];
}

bool Input::getMouseButtonUp(int mouseButton) {
	return mouseUp[mouseButton];
}

bool Input::getMouseButtonHeld(int mouseButton) {
	return mouseHeld[mouseButton];
}

int Input::getMouseScrollDir() {
	return mouseScrollDir;
}

vector Input::getMousePos() {
	return mousePos;
}

bool Input::getKeyDown(int key) {
	return keysDown[key];
}

bool Input::getKeyUp(int key) {
	return keysUp[key];
}

bool Input::getKeyHeld(int key) {
	return keysHeld[key];
}