#pragma once
#include <optional>
#include "Vector3.h"
#include "Ray.h"

struct Box {

	Vector3 boundA, boundB;

	std::optional<float> Intersect(const Ray& ray) const
	{
        Vector3 invRayDirection = 1 / ray.direction;

        // https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
        float tMin, tMax, tyMin, tyMax, tzMin, tzMax;

        Vector3 bounds[2];
        bounds[0] = boundB;
        bounds[1] = boundA;

        int sign[3];
        sign[0] = (invRayDirection.x < 0);
        sign[1] = (invRayDirection.y < 0);
        sign[2] = (invRayDirection.z < 0);


        // Multiplier par l'inverse de la direction afin de ne pas diviser par 0
        tMin = (bounds[sign[0]].x - ray.origin.x) * invRayDirection.x; 
        tMax = (bounds[1-sign[0]].x - ray.origin.x) * invRayDirection.x; 
        tyMin = (bounds[sign[1]].y - ray.origin.y) * invRayDirection.y; 
        tyMax = (bounds[1-sign[1]].y - ray.origin.y) * invRayDirection.y; 

        //tMin = (boundA.x - ray.origin.x) * invRayDirection.x;
        //tMax = (boundA.x - ray.origin.x) * invRayDirection.x;
        //tyMin = (boundB.y - ray.origin.y) * invRayDirection.y;
        //tyMax = (boundB.y - ray.origin.y) * invRayDirection.y;

        if ((tMin > tyMax) || (tyMin > tMax))
            return std::nullopt;

        if (tyMin > tMin)
            tMin = tyMin;
        if (tyMax < tMax)
            tMax = tyMax;

        tzMin = (bounds[sign[2]].z - ray.origin.z) * invRayDirection.z;
        tzMax = (bounds[1 - sign[2]].z - ray.origin.z) * invRayDirection.z;

        //tzMin = (boundA.z - ray.origin.z) * invRayDirection.z;
        //tzMax = (boundB.z - ray.origin.z) * invRayDirection.z;

        if ((tMin > tzMax) || (tzMin > tMax))
            return std::nullopt;
    
        if (tzMin > tMin)
            tMin = tzMin;
        if (tzMax < tMax)
            tMax = tzMax;

        if (0.0 < tMin)
        {
            return tMin;
        }
        if (0.0 < tMax)
        {
            return tMax;
        }

        return std::nullopt;
	}

    template <typename T> int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    Vector3 GetNormal(const Vector3& impactPoint) const
    {
        static const Vector3 normals[] = { // A cube has 3 possible orientations
            Vector3{1,0,0},
            Vector3{0,1,0},
            Vector3{0,0,1},
        };

        const Vector3 center(boundB - boundA);
        const Vector3 interRelative = impactPoint - (center);
        const float xyCoef = interRelative.y / interRelative.x;
        const float zyCoef = interRelative.y / interRelative.z;

        int coef;
        if (-1 <= xyCoef && xyCoef <= 1)
        {
            coef = 1;
        }
        else if (-1 < zyCoef && zyCoef < 1)
        {
            coef = 2;
        }
        else
        {
            coef = 0;
        }

        //float temp (sgn(interRelative));

        // Here it's exclusive to avoid coef to be 3
        return normals[coef];
    }
};