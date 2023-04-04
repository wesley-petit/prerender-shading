#include "Camera.h"

Ray OrthogonalCamera::ScreenToRay(const glm::vec3& pixelPosition) const
{
    Ray ray{ pixelPosition, DIRECTION };
    return ray;
}

Ray PerspectiveCamera::ScreenToRay(const glm::vec3& pixelPosition) const
{
    const glm::vec3 x(pixelPosition - origin);
    const glm::vec3 perspectiveDirection(glm::normalize(x));
    Ray ray{ pixelPosition, perspectiveDirection };

    return ray;
}
