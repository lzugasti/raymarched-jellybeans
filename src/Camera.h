#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <string>
#include <vector>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * This sets camera.
 * By Alexandre Mercier-Aubin
 * Modified by Lo�c Nassif
 */
class Camera
{
public:
	Camera() : fovy(45.0f) {};
	virtual ~Camera() {};

	//position of the camera in the world
	glm::vec3 position = glm::vec3(0, 0, -10);

	//where the camera is pointing at
	glm::vec3 lookAt = glm::vec3(0, 0, 1);

	//a vector that stores in which direction the sky is.
	glm::vec3 up = glm::vec3(0, 1, 0);

	float fovy;
};

#endif
