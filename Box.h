#pragma once
#include <optional>
#include "Ray.h"

struct Box
{
	glm::vec3 boundA, boundB;

    std::optional<float> Intersect(const Ray& ray) const;

    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    glm::vec3 GetNormal(const glm::vec3& impactPoint) const;
};