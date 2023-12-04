#include "Mesh.h"
#include "Ray.h"
#include "IntersectionData.h"

#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

float MESH_EPSILON = 1e-4; // To prevent shadow acne

Mesh::Mesh(const std::string& meshName) : filepath(meshName)
{
	loadOBJ(meshName);
}

Mesh::Mesh(const std::string& meshName, glm::vec3 position) : filepath(meshName)
{
	loadOBJ(meshName);
	translate(position);
}

Mesh::~Mesh()
{
}

void Mesh::translate(glm::vec3 position) 
{
	int j = 0;
	for (int i = 0; i < faceCount; i++) 
	{
		j = 9 * (i+1) - 9;
		// v1
		// posBuf[j + 0] = 1.0f;
		posBuf[j + 0] = posBuf[j + 0] + position.x;
		posBuf[j + 1] = posBuf[j + 1] + position.y;
		posBuf[j + 2] = posBuf[j + 2] + position.z;

		// v2
		posBuf[j + 3] = posBuf[j + 3] + position.x;
		posBuf[j + 4] = posBuf[j + 4] + position.y;
		posBuf[j + 5] = posBuf[j + 5] + position.z;

		// v3
		posBuf[j + 6] = posBuf[j + 6] + position.x;
		posBuf[j + 7] = posBuf[j + 7] + position.y;
		posBuf[j + 8] = posBuf[j + 8] + position.z;
	}
}

void Mesh::scale(float s) 
{
	for (int i = 0; i < faceCount; i++) 
	{
		posBuf[i] = s * posBuf[i];
	}
}

void Mesh::loadOBJ(const std::string& meshName)
{
	// Load geometry
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warnStr, errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnStr, &errStr, meshName.c_str());
	if(!rc) {
		std::cerr << errStr << std::endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
	faceCount = int(posBuf.size() / 9.0f);
}

void Mesh::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
	// TODO 9: Do triangle mesh intersection here
	float bestT = FLT_MAX;
	int j = 0;
	for (int i = 0; i < faceCount; i++)
	{
		j = 9 * (i + 1) - 9;
		// v1
		glm::vec3 a = glm::vec3(posBuf[j + 0], posBuf[j + 1], posBuf[j + 2]);

		// v2
		glm::vec3 b = glm::vec3(posBuf[j + 3], posBuf[j + 4], posBuf[j + 5]);

		// v3
		glm::vec3 c = glm::vec3(posBuf[j + 6], posBuf[j + 7], posBuf[j + 8]);

		glm::vec3 normal = glm::normalize(glm::cross((b - a), (c - a)));

		float tForFace = glm::dot((a - ray->origin), normal) / glm::dot(ray->direction, normal);

		glm::vec3 x;
		ray->computePoint(tForFace, x);

		bool test1 = glm::dot(glm::cross(glm::normalize(b - a), glm::normalize(x - a)), normal) > 0;
		bool test2 = glm::dot(glm::cross(glm::normalize(c - b), glm::normalize(x - b)), normal) > 0;
		bool test3 = glm::dot(glm::cross(glm::normalize(a - c), glm::normalize(x - c)), normal) > 0;

		if ((test1 && test2 && test3) && tForFace > 0.0001f && tForFace < bestT)
		{
			bestT = tForFace;
			intersection->t = tForFace;
			intersection->p = x;
			intersection->n = normal;
			intersection->material = materials[0];
		}
	}
}