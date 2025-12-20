#include"Camera.h"
#include "saveData.h"
#include "character.h"
#include "animation.h"

#include "debugger.h"

Camera* GetMainCamera() {
	return Camera::cameraPtr;
}

Camera::Camera(glm::vec3 position) {
	if (!cameraPtr)
		cameraPtr = this;

	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraMatrix = glm::mat4(1.0f);

	Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
	// Initialize matrices
	//glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	projection = getProjectionMat();
	//cameraMatrix = projection * view;
}

void Camera::Matrix(Shader* shader, const char* uniform) {
	// Exports camera matrix
	shader->setMat4(uniform, cameraMatrix);
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
	vector loc = SaveData::saveData.playerLoc * stuff::pixelSize - stuff::screenSize / 2.f;
	Position = glm::vec3(loc.x, loc.y, 0.f);
	updateMatrix(45.0f, 0.001f, 5000.0f);
}

glm::vec3 Camera::GetPosition() {
	return Position;
}

glm::mat4 Camera::getProjectionMat(vector frameSize) {
	if (frameSize == vector{0, 0})
		frameSize = stuff::screenSize;
	
	float left = 0, right = frameSize.x, bottom = 0, top = frameSize.y;
	return glm::ortho(left, right, bottom, top, -1.f, 5000.0f);
}