#pragma once
struct Shape;

struct RayHit
{
    float distance;
    const Shape* object;
    glm::vec3 impactPoint, normal;
};
