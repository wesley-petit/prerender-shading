#pragma once
#include "Sphere.h"
#include <optional>

struct RayHit
{
	float distance;
	std::optional<const Sphere*> object;
};