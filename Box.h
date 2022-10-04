#pragma once
#include <optional>
#include "Vector3.h"
#include "Ray.h"

struct Box
{
	Vector3 boundA, boundB;

    std::optional<float> Intersect(const Ray& ray) const;

    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    Vector3 GetNormal(const Vector3& impactPoint) const;
};