#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * Material class to specifies surface properties such as diffuse and specular colour. 
 * By Loïc Nassif
 */
class Material
{
public:
	Material() : name(""),
				 diffuse(glm::vec3(1.0f, 1.0f, 1.0f)),
				 specular(glm::vec3(0.0f, 0.0f, 0.0f)),
				 hardness(0.f)
	{
	}
	virtual ~Material() {}

	void reset()
	{
		name = "";
		diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		specular = glm::vec3(0.0f, 0.0f, 0.0f);
		hardness = 0.f;
	}

	std::string name;	// Material name
	glm::vec3 diffuse;	// Diffuse colour. Default white
	glm::vec3 specular; // Specular colour. Default black
	float hardness;		// Specular hardness
	int ID = -1; // ID for material to pair with shapes 
};

#endif
