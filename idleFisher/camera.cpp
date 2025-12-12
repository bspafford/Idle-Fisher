#include"Camera.h"
#include "Input.h"
#include "saveData.h"
#include "character.h"
#include "animation.h"
#include "image.h"

#include "debugger.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	// Define a starting orientation vector (pointing forward)
	Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

	// For a 2:1 isometric view:
	// Use a tilt of 26.565° (from arctan(0.5)) and a Y rotation of 45°.
	// We rotate by -26.565° around the X-axis and -45° around the Y-axis.
	float tiltAngle = 30.f;
	float yRotation = -45.0f;  // we use 45 here since we will pass -yRotation to rotate

	// First, apply the tilt (rotate about the X-axis)
	glm::vec3 tiltedOrientation = glm::rotate(Orientation, glm::radians(-tiltAngle), glm::vec3(1, 0, 0));

	// Next, apply the Y-axis rotation
	Orientation = glm::rotate(tiltedOrientation, glm::radians(-yRotation), Up);

	//Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	Up = glm::vec3(0.0f, -1.0f, 0.0f);
	cameraMatrix = glm::mat4(1.0f);

	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
	// Initialize matrices
	glm::mat4 view = glm::mat4(1.0f);

	// Define camera parameters
	//Position = glm::vec3(10.0f, 10.0f, 10.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);



	// Create the view matrix using lookAt. The camera will look from Position toward Position + Orientation.
	view = glm::lookAt(Position, Position + Orientation, Up);

	// Set up an orthographic projection
	// Define your scale (world units)
	//float left = 0, right = Main::screenSize.x / 10.f, bottom = 0, top = Main::screenSize.y / 10.f;
	float width = stuff::screenSize.x / 10.f;
	float height = stuff::screenSize.y / 10.f;
	float left = -width / 2.f, right = width / 2.f, bottom = -height / 2.f, top = height / 2.f;
	projection = glm::ortho(left, right, bottom, top, nearPlane, farPlane);

	// projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Combine the projection and view matrices to get the final camera matrix
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader* shader, const char* uniform) {
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

glm::vec3 scaleIsometricMovement3D(const glm::vec3& inputXZ) {
	// Using only X and Z components from input (Y remains zero)
	glm::vec3 forwardDir = glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f));
	glm::vec3 rightDir = glm::normalize(glm::vec3(1.0f, 0.0f, -1.0f));

	float forwardAmount = glm::dot(inputXZ, forwardDir);
	float rightAmount = glm::dot(inputXZ, rightDir);

	forwardAmount *= 2.0f;

	glm::vec3 scaledMovement = forwardDir * forwardAmount + rightDir * rightAmount;
	//if (glm::length(scaledMovement) > 1.0f)
	//	scaledMovement = glm::normalize(scaledMovement);

	return scaledMovement;
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
	glm::vec2 animCenter = glm::vec2(Acharacter::anim->cellWidth / 2.f / stuff::pixelSize, Acharacter::anim->cellHeight / stuff::pixelSize);
	glm::vec2 relLoc = glm::vec2(SaveData::saveData.playerLoc.x / 10.f * stuff::pixelSize, SaveData::saveData.playerLoc.y / 5.f * stuff::pixelSize) + animCenter;
	glm::vec3 camLoc = math::convertFromRelativeCoords(relLoc);
	camLoc.y = 50;
	Position = camLoc;

	Inputs(window, deltaTime);
	updateMatrix(45.0f, 0.001f, 5000.0f);
}

void Camera::Inputs(GLFWwindow* window, float deltaTime) {
	//return;

	glm::vec3 move = glm::vec3(0);

	float sqrt2over2 = 0.707106781187;

	// Handles key inputs
	if (Input::getKeyHeld(GLFW_KEY_W)) {
		//move += glm::vec3(-0.353, .866, -0.353);
		//move += glm::vec3(-sqrt2over2, 0.f, -sqrt2over2);
		move += glm::vec3(0, 0, -1);
	}
	if (Input::getKeyHeld(GLFW_KEY_A) == GLFW_PRESS) {
		//move += glm::normalize(glm::cross(Orientation, Up));
		move += glm::vec3(-1, 0, 0);
	}
	if (Input::getKeyHeld(GLFW_KEY_S) == GLFW_PRESS) {
		//move += glm::vec3(0.353, -.866, 0.353);
		//move += glm::vec3(sqrt2over2, 0.f, sqrt2over2);
		move += glm::vec3(0, 0, 1);
	}
	if (Input::getKeyHeld(GLFW_KEY_D) == GLFW_PRESS) {
		//move += -glm::normalize(glm::cross(Orientation, Up));
		move += glm::vec3(1, 0, 0);
	}
	if (Input::getKeyHeld(GLFW_KEY_SPACE) == GLFW_PRESS) {
		move += -Up;
	}
	if (Input::getKeyHeld(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		move += Up;
	}

	glm::vec3 rotated = glm::vec3(0);
	if (glm::length(move))
		move = glm::normalize(move);

	move.z *= 2;
	rotated = glm::rotate(move, glm::radians(45.f), glm::vec3(0, 1, 0));
	Position += speed * rotated * deltaTime;

	if (false) {
		// Handles mouse inputs
		if (Input::getMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			// Hides mouse cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

			// Prevents camera from jumping on the first click
			if (firstClick) {
				glfwSetCursorPos(window, (width / 2), (height / 2));
				firstClick = false;
			}

			// Stores the coordinates of the cursor
			double mouseX;
			double mouseY;
			// Fetches the coordinates of the cursor
			glfwGetCursorPos(window, &mouseX, &mouseY);

			// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
			// and then "transforms" them into degrees 
			float rotX = -sensitivity * (float)(mouseY - (float(height) / 2.f)) / float(height);
			float rotY = -sensitivity * (float)(mouseX - (width / 2)) / width;

			// Calculates upcoming vertical change in the Orientation
			glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

			// Decides whether or not the next vertical Orientation is legal or not
			if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
				Orientation = newOrientation;
			}

			// Rotates the Orientation left and right
			Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);


			// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
			glfwSetCursorPos(window, (width / 2), (height / 2));
		} else if (Input::getMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) {
			// Unhides cursor since camera is not looking around anymore
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			// Makes sure the next time the camera looks around it doesn't jump
			firstClick = true;
		}
	}
}

void Camera::printCamRot() {
	float radToDeg = 180 / 3.1415926;

	float yaw = atan2(cameraMatrix[0][1], cameraMatrix[0][0]) * radToDeg;
	float pitch = atan2(-cameraMatrix[0][2], sqrt(cameraMatrix[0][0] * cameraMatrix[0][0] + cameraMatrix[0][1] * cameraMatrix[0][1])) * radToDeg;
	float roll = atan2(cameraMatrix[1][2], cameraMatrix[2][2]) * radToDeg;

	std::cout << "yaw: " << yaw << ", pitch: " << pitch << ", roll: " << roll << std::endl;
}

glm::mat4 Camera::getProjectionMat(vector frameSize) {
	if (frameSize == vector{0, 0})
		frameSize = stuff::screenSize;
	
	float left = -frameSize.x / 2.f, right = frameSize.x / 2.f, bottom = -frameSize.y / 2.f, top = frameSize.y / 2.f;

	return glm::ortho(left, right, bottom, top, -1.f, 5000.0f);
}