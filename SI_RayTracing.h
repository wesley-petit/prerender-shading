#pragma once
#include "Ray.h"
#include "RayHit.h"

float GetOutgoingLightFor(const Sphere& current, const Ray& ray, float distanceWithCamera, LightSource& lightSource);

RayHit& GetNearestHit(std::vector<Sphere>& spheres, const Ray& ray);
