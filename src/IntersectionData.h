#pragma once
#ifndef INTERSECTIONDATA_H
#define INTERSECTIONDATA_H

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.h"

/**
 * Convenient class to hold information of the closest intersection for a given ray. 
 * By Loïc Nassif
 */
class IntersectionData
{
public:
	IntersectionData() 
	{
		material = std::make_shared<Material>();
	}
	virtual ~IntersectionData() {}

	void reset()
	{
		n = glm::vec3(0.0f, 0.0f, 0.0f);
		p = glm::vec3(0.0f, 0.0f, 0.0f);
		t = FLT_MAX;
		material->reset();
	}

	glm::vec3 n = glm::vec3(0.0f, 0.0f, 0.0f); // Intersection normal
	glm::vec3 p = glm::vec3(0.0f, 0.0f, 0.0f); // Intersection point
	float t = FLT_MAX; // Ray parameter giving the position of intersection

	std::shared_ptr<Material> material;
};

#endif
