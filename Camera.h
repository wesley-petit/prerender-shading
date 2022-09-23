#pragma once
#include "Vector3.h"
#include "Ray.h"

// Orthogonal camera
class OrthogonalCamera
{
public:
	// Construit un raycast à partir de la position d'un pixel
	Ray& ScreenToRay(const Vector3& pixelPosition) const;
};
