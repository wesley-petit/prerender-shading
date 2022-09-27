#pragma once
#include "Sphere.h"

struct RayHit
{
    float distance;
    const Sphere* object;
    Vector3 impactPoint, normal;
};
