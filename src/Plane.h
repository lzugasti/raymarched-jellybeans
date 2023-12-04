#pragma once
#ifndef PLANE_H
#define PLANE_H

#include "Shape.h"
#include "Material.h"


/**
A plane object. Requires a normal and a point on the plane to be defined. 
By Loïc Nassif
*/
class Plane : public Shape
{
public:
	Plane();
	Plane(glm::vec3 _normal, glm::vec3 _position);
	Plane(glm::vec3 _normal);

	virtual ~Plane();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	glm::vec3 normal;
	glm::vec3 position;
};

#endif
