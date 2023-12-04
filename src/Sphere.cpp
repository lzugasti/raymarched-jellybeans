#include "Sphere.h"
#include "Ray.h"
#include "IntersectionData.h"

float SPHERE_EPSILON = 1e-4; // To prevent shadow acne

Sphere::Sphere() :
    radius(1.0f),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::Sphere(float _radius, glm::vec3 _center) :
    radius(_radius),
    center(_center)
{
}

Sphere::Sphere(float _radius) :
    radius(_radius),
    center(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

Sphere::~Sphere()
{
}

void Sphere::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    glm::vec3 eminusc = ray->origin - center;
    float dtimesp = glm::dot(ray->direction, eminusc);
    float dtimesd = glm::dot(ray->direction, ray->direction);
    float ptimesp = glm::dot(eminusc, eminusc);
    float squarerootinput = dtimesp * dtimesp - dtimesd * (ptimesp - radius * radius);
    float t = 0;
    if (squarerootinput < 0)
    {
        return;
    }
    if (squarerootinput == 0)
    {
        t = -dtimesp / dtimesd;
    }
    if (squarerootinput > 0)
    {
        float negativeT = (-dtimesp - glm::sqrt(squarerootinput)) / dtimesd;
        float positiveT = (-dtimesp + glm::sqrt(squarerootinput)) / dtimesd;
        t = glm::min(glm::max(negativeT, 0.0f), glm::max(positiveT, 0.0f));
    }

    if (t <= 0.0001f)
    {
        return;
    }

    ray->computePoint(t, intersection->p);
    intersection->n = glm::normalize(intersection->p - center);
    intersection->t = t;

    intersection->material = materials[0];
}