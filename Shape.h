#pragma once
#include <optional>

#include <glm/glm.hpp>
#include "Ray.h"
#include "ReflectionType.h"

struct Shape
{
    virtual std::optional<float> Intersect(const Ray& ray) const = 0;
    virtual glm::vec3 GetNormal(const glm::vec3& impactPoint) const = 0;
    virtual ReflectionType GetReflectionType() const = 0;
};

template<typename T>
struct AnyShape : Shape
{
    T obj;

    AnyShape(T newObj) : obj(newObj) {};

    // Hérité via Shape
    virtual std::optional<float> Intersect(const Ray& ray) const override
    {
        return obj.Intersect(ray);
    }

    virtual glm::vec3 GetNormal(const glm::vec3& impactPoint) const override
    {
        return obj.GetNormal(impactPoint);
    }

    virtual ReflectionType GetReflectionType() const override
    {
        return obj.GetReflectionType();
    }
};