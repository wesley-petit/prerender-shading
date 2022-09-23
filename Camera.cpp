#include "Camera.h"

Ray& OrthogonalCamera::ScreenToRay(const Vector3& pixelPosition) const
{
	const Vector3 direction{ Vector3{0, 0, -1}  };
	Ray ray{pixelPosition, direction};
	return ray;
}
