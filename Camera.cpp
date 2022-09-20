#include "Camera.h"

Ray& Camera::ScreenToRay(Vector3& const position) const
{
	const Vector3 direction{ Vector3{0, 0, 1}  };
	Ray ray{position, direction};
	return ray;
}
