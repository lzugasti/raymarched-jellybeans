#include "Scene.h"
#include "Ray.h"
#include "IntersectionData.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"
#include "iostream"

#include <stdlib.h> 

float SHADOW_EPSILON = 1e-6; // To prevent shadow acne

// Default values, they get overwritten if scene file contains them
Scene::Scene(const std::string name) :
	width(1080), height(720), aspect(1080.0f / 720.0f),
	shininess(64),
	ambient(0.1f, 0.1f, 0.1f)
{
	outputFilename = name;
}

void Scene::init()
{
	// Initialize camera	
	cam = make_shared<Camera>();

	// Init image
	image = make_shared<Image>(width, height);
}

Scene::~Scene()
{
}

void Scene::addObject(shared_ptr<Shape> shape)
{
	shapes.push_back(shape);
}

void Scene::addLight(shared_ptr<Light> light)
{
	lights.push_back(light);
}

void Scene::render()
{
	std::shared_ptr<IntersectionData> intersection = make_shared<IntersectionData>();
	std::shared_ptr<IntersectionData> shadowIntersection = make_shared<IntersectionData>();
	glm::vec3 camDir = -cam->lookAt + cam->position;

	// Compute basis
	glm::vec3 w = glm::normalize(camDir);
	glm::vec3 u = glm::normalize(glm::cross(cam->up, w));
	glm::vec3 v = glm::cross(w, u);

	std::shared_ptr<Ray> ray = make_shared<Ray>();
	std::shared_ptr<Ray> shadowRay = make_shared<Ray>();

	//for dof blur
	std::shared_ptr<Ray> secondaryRay = make_shared<Ray>();

	float d = 1;
	float top = d * glm::tan(0.5 * M_PI * cam->fovy / 180.f);
	float right = aspect * top;
	float bottom = -top;
	float left = -right;

	ray->origin = cam->position;

	glm::vec3 pixel = glm::vec3();
	glm::vec3 colour = glm::vec3();

	for (int i = 0; i < width; i++)
	{
		std::cout << i << std::endl;
		for (int j = 0; j < height; j++)
		{
			colour = glm::vec3(0.0f, 0.0f, 0.0f);

			if (!dofBlur)
			{
				// Anti aliasing and jittering
				for (int p = 0; p < samples; p++)
				{
					for (int q = 0; q < samples; q++)
					{
						float randomX = 0;
						float randomY = 0;

						if (jitter)
						{
							randomX = rand() / (float)RAND_MAX;
							randomY = rand() / (float)RAND_MAX;
						}
						float xOffset = i + ((float)p + randomX) / samples;
						float yOffset = j + ((float)q + randomY) / samples;

						// TODO: 1. generate a ray
						float uScalar = left + (right - left) * (xOffset + 0.5f) / width;
						float vScalar = bottom + (top - bottom) * (yOffset + 0.5f) / height;
						glm::vec3 s = cam->position + uScalar * u + vScalar * v - d * w;
						ray->direction = glm::normalize(s - ray->origin);

						colour += generateRay(w, u, v, ray, shadowRay, intersection, shadowIntersection);
					}
				}
				colour = colour / (float)(samples * samples);
			}
			else
			{
				// TODO: 1. generate a ray
				float uScalar = left + (right - left) * (i + 0.5f) / width;
				float vScalar = bottom + (top - bottom) * (j + 0.5f) / height;
				glm::vec3 s = cam->position + uScalar * u + vScalar * v - d * w;
				ray->direction = glm::normalize(s - ray->origin);

				glm::vec3 convergence = glm::vec3(0.0f, 0.0f, 0.0f);
				ray->computePoint(focalLength, convergence);

				glm::vec3 mainColour = generateRay(w, u, v, ray, shadowRay, intersection, shadowIntersection);
				for (int p = 0; p < dofSamples; p++)
				{
					for (int q = 0; q < dofSamples; q++)
					{
						float randomX = 0;
						float randomY = 0;
						float randomZ = 0;

						if (dofJitter)
						{
							randomX = ((rand() / (float)RAND_MAX) - 0.5f) / dofVariation;
							randomY = ((rand() / (float)RAND_MAX) - 0.5f) / dofVariation;
							randomZ = ((rand() / (float)RAND_MAX) - 0.5f) / dofVariation;
						}
						else
						{
							float x = (p - (dofSamples / 2.0f)) / dofSamples;
							float y = (q - (dofSamples / 2.0f)) / dofSamples;
							randomX = x / dofVariation;
							randomY = y / dofVariation;
							randomZ = 0;
						}

						secondaryRay->origin = cam->position + glm::vec3(randomX, randomY, randomZ);
						secondaryRay->direction = glm::normalize(convergence - secondaryRay->origin);
						intersection->t = -1;

						colour += generateRay(w, u, v, secondaryRay, shadowRay, intersection, shadowIntersection);
					}
				}
				colour += mainColour;
				colour = colour / (float)(dofSamples * dofSamples + 1);
			}


			// Clamp colour values to 1
			colour.r = glm::min(1.0f, colour.r);
			colour.g = glm::min(1.0f, colour.g);
			colour.b = glm::min(1.0f, colour.b);

			// Write pixel colour to image
			colour *= 255;
			image->setPixel(i, j, colour.r, colour.g, colour.b);
		}
	}

	image->writeToFile(outputFilename);
}

glm::vec3 Scene::generateRay(glm::vec3 w, glm::vec3 u, glm::vec3 v, std::shared_ptr<Ray> ray, std::shared_ptr<Ray> shadowRay, std::shared_ptr<IntersectionData> intersection, std::shared_ptr<IntersectionData> shadowIntersection, int depth)
{

	glm::vec3 eyeVec = glm::vec3();
	glm::vec3 lightVec = glm::vec3();
	glm::vec3 h = glm::vec3(); // Bisection vector

	glm::vec3 colour = glm::vec3();


	// keep track if we found a shape
	bool hitSomething = false;
	// set to black if there isn't any collisions
	colour = glm::vec3(0.0f, 0.0f, 0.0f);

	// TODO: 2. test for intersection with scene surfaces
	for (int k = 0; k < shapes.size(); k++)
	{
		// Try to intersect with the shape
		std::shared_ptr<IntersectionData> temporaryIntersection = make_shared<IntersectionData>();
		temporaryIntersection->t = -1;
		shapes[k]->intersect(ray, temporaryIntersection);

		// first check if we hit anything in front of us;
		// then check that what we hit is closer to us than any previous intersections (if there are any)
		if (temporaryIntersection->t > 0.0001f
			&& (!hitSomething || glm::length(temporaryIntersection->p - cam->position) < glm::length(intersection->p - cam->position)))
		{

			// TODO: 3. compute the shaded result for the intersection point
			hitSomething = true;
			intersection = temporaryIntersection;

			// lighting
			glm::vec3 scatteredLight = ambient;
			glm::vec3 reflectedLight = glm::vec3(0.0f, 0.0f, 0.0f);

			eyeVec = glm::normalize(cam->position - intersection->p);
			if (shapes[k]->mirrorReflection)
			{
				std::shared_ptr<Ray> reflectionRay = make_shared<Ray>();
				glm::vec3 reflectionRayDirection = glm::normalize(2 * (glm::dot(intersection->n, eyeVec)) * intersection->n - eyeVec);
				reflectionRay->direction = reflectionRayDirection;
				reflectionRay->origin = intersection->p;
				if (depth > 0)
				{
					reflectedLight += shapes[k]->mirrorReflectionIntensity * generateRay(w, u, v, reflectionRay, shadowRay, intersection, shadowIntersection, depth - 1);
				}
			}

			for (int l = 0; l < lights.size(); l++)
			{
				shadowRay->direction = glm::normalize(lights[l]->position - intersection->p);
				shadowRay->origin = intersection->p;

				shadowIntersection->t = -1;
				bool shadowHitSomething = false;
				for (int s = 0; s < shapes.size(); s++)
				{
					shapes[s]->intersect(shadowRay, shadowIntersection);
					// if hit something then it's in shadow
					if (shadowIntersection->t > 0.0001f)
					{
						shadowHitSomething = true;
						break;
					}
				}
				if (shadowHitSomething)
				{
					continue;
				}

				float diffuse = 0.0f;
				float specular = 0.0f;
				float distanceModifier = lights[l]->power;
				if (lights[l]->type.compare("point") == 0)
				{
					lightVec = lights[l]->position - intersection->p;
					distanceModifier *= 1.0f / (.7f + 0.05f * glm::length(lightVec) + 0.00002f * glm::dot(lightVec, lightVec));//150.0f * lights[l]->power / glm::dot(lightVec, lightVec);
					lightVec = glm::normalize(lightVec);
				}
				else if (lights[l]->type.compare("directional") == 0)
				{
					lightVec = glm::normalize(lights[l]->position);
				}

				h = glm::normalize(lightVec + eyeVec);

				diffuse += glm::max(0.0f, glm::dot(intersection->n, lightVec));
				specular += glm::max(0.0f, glm::dot(intersection->n, h));

				if (diffuse == 0.0f)
				{
					specular = 0.0f;
				}
				else
				{
					specular = glm::pow(specular, glm::max(1.0f, intersection->material->hardness));
				}

				scatteredLight += distanceModifier * lights[l]->colour * glm::max(0.0f, diffuse);
				reflectedLight += distanceModifier * lights[l]->colour * glm::max(0.0f, specular);

			}
			colour = glm::min(intersection->material->diffuse * scatteredLight + reflectedLight, glm::vec3(1.0f, 1.0f, 1.0f));
		}
	}

	return colour;

}
