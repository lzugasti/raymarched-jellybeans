#include "Plane.h"
#include "Ray.h"
#include "IntersectionData.h"

float PLANE_EPSILON = 1e-4; // To prevent shadow acne

Plane::Plane() :
    normal(glm::vec3(0.0f, 1.0f, 0.0f)),
    position(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Plane::Plane(glm::vec3 _normal, glm::vec3 _position) :
    normal(_normal),
    position(_position)
{
}

Plane::Plane(glm::vec3 _normal) :
    normal(_normal), position(0.0f, 0.0f, 0.0f)
{
}

Plane::~Plane()
{
}

void Plane::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 4: intersection of ray with plane
    float t = glm::dot((position - ray->origin), normal) / glm::dot(ray->direction, normal);

    if (t <= 0.0001f)
    {
        return;
    }

    intersection->n = normal;
    intersection->t = t;

    ray->computePoint(intersection->t, intersection->p);

    int x = (int)intersection->p.x;
    int z = (int)intersection->p.z;
    if (intersection->p.x >= 0 && intersection->p.z >= 0 || intersection->p.x <= 0 && intersection->p.z <= 0)
    {
        intersection->material = materials[glm::abs(x + z) % 2];
    }
    else if (intersection->p.x <= 0 && intersection->p.z >= 0 || intersection->p.x >= 0 && intersection->p.z <= 0)
    {
        intersection->material = materials[glm::abs(x + z + 1) % 2];
    }
}