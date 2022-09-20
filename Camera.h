#pragma once
#include "Vector3.h"
#include "Ray.h"

// Orthogonal camera
class Camera
{
public:
	Ray& ScreenToRay(Vector3& const position) const;
};
