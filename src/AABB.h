#pragma once
#ifndef AABB_H
#define AABB_H

#include "Shape.h"
#include "Material.h"


/**
 * An axis aligned bounding box. Used to render boxes. 
 * By Loïc Nassif
 */
class AABB : public Shape
{
public:
	AABB();
	AABB(glm::vec3 size, glm::vec3 position);

	virtual ~AABB();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	glm::vec3 minpt;
	glm::vec3 maxpt;

};

#endif