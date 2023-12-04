#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <memory>

#include "Shape.h"
#include "Material.h"

/**
 * A shape defined by a list of triangles
 * - posBuf should be of length 3*ntris
 * - norBuf should be of length 3*ntris (if normals are available)
 * - texBuf should be of length 2*ntris (if texture coords are available)
 */
class Mesh : public Shape
{
public:
	Mesh(const std::string& meshName);
	Mesh(const std::string& meshName, glm::vec3 position);

	virtual ~Mesh();
	void loadOBJ(const std::string &meshName);
	
	void intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection);

	void translate(glm::vec3 position);

	void scale(float s); // Uniform scale

	std::string filepath;

private:
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;

	int faceCount = 0;
};

#endif
