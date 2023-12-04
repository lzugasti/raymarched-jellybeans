#pragma once
#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "Material.h"

/**
A sphere class. It is specified with a radius and a center position.
- By Loïc Nassif
 */
class Sphere : public Shape
{
public:
	Sphere();
	Sphere(float _radius, glm::vec3 _center);
	Sphere(float _radius);

	virtual ~Sphere();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	float radius;
	glm::vec3 center;
};

#endif
