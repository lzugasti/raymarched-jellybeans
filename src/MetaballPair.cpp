#include "MetaballPair.h"
#include "Ray.h"
#include "IntersectionData.h"

MetaballPair::MetaballPair() :
    radius(1.0f),
    center1(glm::vec3(0.0f, 0.0f, 0.0f)),
    center2(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

MetaballPair::MetaballPair(float _radius, glm::vec3 _center1, glm::vec3 _center2) :
    radius(_radius),
    center1(_center1),
    center2(_center2)
{
}

MetaballPair::MetaballPair(float _radius) :
    radius(_radius),
    center1(glm::vec3(-_radius, 0.f, 0.f)),
    center2(glm::vec3(_radius, 0.f, 0.f))
{
}

MetaballPair::~MetaballPair()
{
}

float MetaballPair::distance(glm::vec3 pos)
{
    float distance1 = glm::length(center1 - pos) - radius;
    float distance2 = glm::length(center2 - pos) - radius;
    float d = smoothMin(distance1, distance2, 6.0f);
    return d;
}

float MetaballPair::smoothMin(float a, float b, float k) {
    float res = glm::exp2(-k * a) + glm::exp2(-k * b);
    return -glm::log2(res) / k;
}

void MetaballPair::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    glm::vec3 currentPosition = ray->origin;
    float d = 0;
    float total = 0;
    do
    {
        d = distance(currentPosition);
        total += d;
        ray->computePoint(total, currentPosition);

    } while (total < 20 && d > 0.0001f);

    if (total < 20)
    {
        intersection->p = currentPosition;
        intersection->t = total;
        intersection->material = materials[0];

        float eps = 0.0001f;
        glm::vec3 smallX = glm::vec3(eps, 0.0f, 0.0f);
        glm::vec3 smallY = glm::vec3(0.0f, eps, 0.0f);
        glm::vec3 smallZ = glm::vec3(0.0f, 0.0f, eps);
        glm::vec3 normal = glm::vec3(
            distance(currentPosition + smallX) - distance(currentPosition - smallX), 
            distance(currentPosition + smallY) - distance(currentPosition - smallY),
            distance(currentPosition + smallZ) - distance(currentPosition - smallZ));
        intersection->n = glm::normalize(normal);

    }
    

}