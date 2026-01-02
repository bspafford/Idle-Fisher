#pragma once

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include "math.h"
#include"shaderClass.h"
#include "stuff.h"

class Camera {
public:
	// Camera constructor to set up initial values
	Camera(glm::vec3 position = glm::vec3(0.f));

	// Frame size is relative to what window, by default its the screen size, change it if using something like a FBO
	static glm::mat4 getProjectionMat(vector frameSize = { 0, 0 });

	void Update(GLFWwindow* window, float deltaTime);

	glm::vec3 GetPosition();

	friend Camera* GetMainCamera();

private:
	// Stores the main vectors of the camera
	glm::vec3 position;
	float speed = 37.5f;

	static inline Camera* cameraPtr;
};

Camera* GetMainCamera();
