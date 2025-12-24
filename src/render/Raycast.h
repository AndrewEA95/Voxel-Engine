#pragma once
#include "Ray.h"
#include "../../extern/glm-1.0.2/glm/glm.hpp"

inline bool intersectRayAABB(
    const Ray& ray,
    const glm::mat4& world,
    const glm::vec3& min,
    const glm::vec3& max,
    float& outDist)
{
    glm::mat4 inv = glm::inverse(world);
    glm::vec3 ro = glm::vec3(inv * glm::vec4(ray.origin, 1.0f));
    glm::vec3 rd = glm::normalize(glm::vec3(inv * glm::vec4(ray.direction, 0.0f)));

    float tmin = (min.x - ro.x) / rd.x;
    float tmax = (max.x - ro.x) / rd.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (min.y - ro.y) / rd.y;
    float tymax = (max.y - ro.y) / rd.y;
    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return false;

    tmin = glm::max(tmin, tymin);
    tmax = glm::min(tmax, tymax);

    float tzmin = (min.z - ro.z) / rd.z;
    float tzmax = (max.z - ro.z) / rd.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    tmin = glm::max(tmin, tzmin);

    outDist = tmin;
    return true;
}