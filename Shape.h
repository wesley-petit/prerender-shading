#pragma once
#include <optional>

struct Ray;
struct Vector3;

struct Shape
{
    virtual std::optional<float> Intersect(const Ray& ray) const = 0;
    virtual Vector3 GetNormal(const Vector3& impactPoint) const = 0;
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

    virtual Vector3 GetNormal(const Vector3& impactPoint) const override
    {
        return obj.GetNormal(impactPoint);
    }
};