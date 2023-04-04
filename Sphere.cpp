#include <cmath>
#include <functional>
#include "CustomMath.h"
#include "Ray.h"
#include "Sphere.h"

std::optional<float> Sphere::Intersect(const Ray& ray) const
{
    // L'intersection est une �quation de second degr�
    // il faut calculer les param�tres b et c pour d�terminer l'inconnu t

    //b = dot direction oc
    const glm::vec3 oc{ center - ray.origin };
    const float b(glm::dot(ray.direction, oc));

    //c = dot oc oc - r2
    const float c(glm::dot(oc, oc) - radius * radius);

    // delta = b� - 4ac, mais 4a ont pus �tre simplifi�s
    const float delta = b * b - c;

    //std::cout << "B : " << b << std::endl;
    //std::cout << "C : " << c << std::endl;
    //std::cout << "Delta : " << delta << std::endl;

    if (delta < 0 && !IsFloatEqual(delta, 0.0))
    {
        return std::nullopt;
    }

    // t = (- b +- (sqrt(delta)) / 2A
    //t0 = b - sqrtDelta
    //t1 = b + sqrtDelta
    const float sqrtDelta(sqrt(delta));
    const float t0(b - sqrtDelta);
    const float t1(b + sqrtDelta);

    //std::cout << "T0 : " << t0 << std::endl;
    //std::cout << "T1 : " << t1 << std::endl;

    if (0.0 < t0)
    {
        return t0;
    }
    if (0.0 < t1)
    {
        return t1;
    }

    return std::nullopt;
}

glm::vec3 Sphere::GetNormal(const glm::vec3& impactPoint) const
{
    return { glm::normalize(impactPoint - center) };
}

ReflectionType Sphere::GetReflectionType() const
{
    return reflectionType;
}
