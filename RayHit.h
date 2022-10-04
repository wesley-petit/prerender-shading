#pragma once
struct Shape;

struct RayHit
{
    float distance;
    const Shape* object;
    Vector3 impactPoint, normal;
};
