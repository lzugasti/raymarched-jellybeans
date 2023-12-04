#pragma once
#ifndef METABALLPAIR_H
#define METABALLPAIR_H

#include "Shape.h"
#include "Material.h"

/**
A sphere class. It is specified with a radius and a center position.
- By Loïc Nassif
 */
class MetaballPair : public Shape
{
public:
	MetaballPair();
	MetaballPair(float _radius);
	MetaballPair(float _radius, glm::vec3 _center1, glm::vec3 _center2);

	virtual ~MetaballPair();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	float radius;
	glm::vec3 center1;
	glm::vec3 center2;
private:
	float distance(glm::vec3 pos);
	float smoothMin(float a, float b, float k);
};

#endif
