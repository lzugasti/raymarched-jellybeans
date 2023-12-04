#include "HierachicalShape.h"
#include "Ray.h"
#include "IntersectionData.h"

float HIERACHY_EPSILON = 1e-4; // To prevent shadow acne

#include <iostream>

HierachicalShape::HierachicalShape() :
    M(glm::mat4(1.0f)), Minv(glm::mat4(1.0f))
{
    transformRay = std::make_shared<Ray>();
    transformData = std::make_shared<IntersectionData>();
}

HierachicalShape::~HierachicalShape()
{
}

void HierachicalShape::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO 7: do something useful here!
    transformRay->direction = Minv * glm::vec4(ray->direction.x, ray->direction.y, ray->direction.z, 0.0);
    transformRay->origin = Minv * glm::vec4(ray->origin.x, ray->origin.y, ray->origin.z, 1.0);

    float bestT = FLT_MAX;
    for (int i = 0; i < children.size(); i++)
    {
        transformData->t = -1;
        children[i]->intersect(transformRay, transformData);
        if (transformData->t > 0.0001f && transformData->t < bestT)
        {
            bestT = transformData->t;
            intersection->t = transformData->t;
            intersection->n = transformData->n;
            intersection->p = transformData->p;
            intersection->material = transformData->material;
        }
    }

    intersection->n = glm::normalize(M * glm::vec4(intersection->n, 0.0f));
    intersection->p = M * glm::vec4(intersection->p, 1.0f);
    if (intersection->material == NULL)
    {
        intersection->material = materials[0];
    }
}