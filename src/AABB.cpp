#include "AABB.h"
#include "Ray.h"
#include "IntersectionData.h"

static float AABB_EPSILON = 1e-4; // To prevent shadow acne
float t_min = FLT_MIN;
float t_max = FLT_MAX;

AABB::AABB() :
    minpt(0.0f, 0.0f, 0.0f),
    maxpt(10.0f, 10.0f, 10.0f)
{
}

AABB::AABB(glm::vec3 size, glm::vec3 position)
{
    // Compute the min and max points from the size and com
    minpt = position - size / 2.0f;
    maxpt = position + size / 2.0f;
}

AABB::~AABB()
{
}

void AABB::intersect(const std::shared_ptr<Ray> ray, std::shared_ptr<IntersectionData> intersection)
{
    // TODO: Objective 6: intersection of Ray with axis aligned box
    float tminx = (minpt.x - ray->origin.x) / ray->direction.x;
    float tmaxx = (maxpt.x - ray->origin.x) / ray->direction.x;

    float txlow = glm::min(tminx, tmaxx);
    float txhigh = glm::max(tminx, tmaxx);

    float tminy = (minpt.y - ray->origin.y) / ray->direction.y;
    float tmaxy = (maxpt.y - ray->origin.y) / ray->direction.y;

    float tylow = glm::min(tminy, tmaxy);
    float tyhigh = glm::max(tminy, tmaxy);

    float tminz = (minpt.z - ray->origin.z) / ray->direction.z;
    float tmaxz = (maxpt.z - ray->origin.z) / ray->direction.z;

    float tzlow = glm::min(tminz, tmaxz);
    float tzhigh = glm::max(tminz, tmaxz);

    float tmin = glm::max(glm::max(txlow, tylow), tzlow);
    float tmax = glm::min(glm::min(txhigh, tyhigh), tzhigh);

    if (tmax < tmin) return;

    if (tmin == txlow) {
        if (txlow == tminx)
        {
            intersection->n = glm::vec3(-1.0f, 0.0f, 0.0f);
        }
        else
        {
            intersection->n = glm::vec3(1.0f, 0.0f, 0.0f);
        }
    }
    if (tmin == tylow) {
        if (tylow == tminy)
        {
            intersection->n = glm::vec3(0.0f, -1.0f, 0.0f);
        }
        else
        {
            intersection->n = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
    if (tmin == tzlow) {
        if (tzlow == tminz)
        {
            intersection->n = glm::vec3(0.0f, 0.0f, -1.0f);
        }
        else
        {
            intersection->n = glm::vec3(0.0f, 0.0f, 1.0f);
        }
    }
    if (tmin <= 0.0001f)
    {
        return;
    }
    intersection->t = tmin;
    ray->computePoint(tmin, intersection->p);

    intersection->material = materials[0];
}