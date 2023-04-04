#pragma once
#include <optional>
#include "ReflectionType.h"

struct Sphere
{
    glm::vec3 center;
    float radius;
    ReflectionType reflectionType;

    std::optional<float> Intersect(const Ray& ray) const;
    glm::vec3 GetNormal(const glm::vec3& impactPoint) const;
    ReflectionType GetReflectionType() const;
};


// ---------------------------------------------- Pseudo Code de l'intersection ---------------------------------------------- 
//```haskell
//rayIntersectSphere::Ray->Sphere->Maybe Float
//rayIntersectSphere Ray{ origin, direction } Sphere{ radius, center } =
//let-- >> > A point X on a ray is :
//--
//--X = O + t * D
//--
//-- >> > A point X on a sphere is :
//--
//--distance(X, C) = R
//--
//-- >> > We can inject X in the second equation :
//--
//--distance(O + t * D, C) = R
//--
//-- >> > And replace distance(A, B) by length(A - B)
//--
//--length(O + t * D - C) = R
//--
//-- >> > We can square both sides
//--
//--length(t * D + O - C) ^ 2 = R ^ 2
//--
//-- >> > introduces some simplifications
//
//oc = origin-- > center
//r2 = radius * radius
//-- length(t * D - OC) ^ 2 = r2
//--
//-- >> > let dot'(A) = dot(A, A) = length(A)^2
//--
//--dot'(t * D - OC) = R2
//--
//-- >> > Distribute
//-- t ^ 2 * dot'(D) + dot'(OC)-2 * t * dot(D, OC) = R2
//--
//-- >> > Let group terms with respect to t
//--
//--t ^ 2 * dot'(D) +
//--t * (-2 * dot(D, OC)) +
//--dot'(OC) - R2
//-- = 0
//--
//-- >> > This is a second order equation which can be solved using
//-- >> > https://en.wikipedia.org/wiki/Quadratic_equation#Discriminant
//--t ^ 2 * a + t * b + c = 0
//
//--a = dot direction direction
//-- We know by construction that the ray direction is normalized, so a = 1
//
//--b = -2 * dot direction oc
//-- We simplify the next expressions by removing the division by 2
//--We can also remove the negation of b
//b = dot direction oc
//c = dot oc oc - r2
//
//-- delta = b * b - 4 * a * c
//-- 4 and a are removed based on previous optimisations
//delta = b * b - c
//
//-- There is two solutions, t0 and t1, if delta >= 0
//sqrtDelta = sqrt delta
//t0 = b - sqrtDelta
//t1 = b + sqrtDelta
//in if
//--TODO: check if removing this test is good for performances
//| delta < 0->Nothing -- No solution, the ray missed the sphere
//    | t0 >= 0->Just t0 -- t0 solution is the smallest(by construction), and is positive.The ray hit the front of the sphere.
//    | t1 >= 0->Just t1 -- t1 solution is the smallest positive.The ray started inside the sphere and exits it.
//    | otherwise->Nothing -- neither t0 or t1 are positive, the ray is starting after the sphere.
//    ```
