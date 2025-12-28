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
	this->position = position;
}

void Camera::Update(GLFWwindow* window, float deltaTime) {
	vector loc = SaveData::saveData.playerLoc * stuff::pixelSize - stuff::screenSize / 2.f;
	position = glm::vec3(loc.x, loc.y, 0.f);

	projection = getProjectionMat();
}

glm::vec3 Camera::GetPosition() {
	return position;
}

glm::mat4 Camera::getProjectionMat(vector frameSize) {
	if (frameSize == vector{0, 0})
		frameSize = stuff::screenSize;
	
	float left = 0, right = frameSize.x, bottom = 0, top = frameSize.y;
	return glm::ortho(left, right, bottom, top, -1.f, 5000.0f);
}