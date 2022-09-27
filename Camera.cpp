#include "Camera.h"

Ray& OrthogonalCamera::ScreenToRay(const Vector3& pixelPosition) const
{
    Ray ray{ pixelPosition, DIRECTION };
    return ray;
}

Ray& PerspectiveCamera::ScreenToRay(const Vector3& pixelPosition) const
{
    const Vector3 x(pixelPosition - origin);
    const Vector3 perspectiveDirection(x.unitVector());
    Ray ray{ pixelPosition, perspectiveDirection };

    return ray;
}
