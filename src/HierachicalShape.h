#pragma once
#ifndef HIERACHICALSHAPE_H
#define HIERACHICALSHAPE_H

#include "Shape.h"
#include "Material.h"

#include <vector>

/**
 * The scene is constructed from a hierarchy of nodes, where each node
 * contains a transform, a material definition, some amount of geometry,
 * and some number of children nodes.  Each node has a unique name so that
 * it can be instanced elsewhere in the hierarchy (provided it does not
 * make loops.
 *
 * Note that if the material for a scene node is non-null, it should override
 * the intersect result material of any child.
 *  
 * By Loïc Nassif
 * Adapted from Paul Kry's code.
 */
class HierachicalShape : public Shape
{
public:
	HierachicalShape();

	virtual ~HierachicalShape();

	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Shape>> children;

	glm::mat4 M; // Matrix transform
	glm::mat4 Minv; // Matrix transform inverse

private:
	std::shared_ptr<Ray> transformRay;
	std::shared_ptr<IntersectionData> transformData;
};

#endif
