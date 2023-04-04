#pragma once
#include <glm/glm.hpp>
#include "Ray.h"

class Camera
{
public:
    virtual ~Camera() = default;
    virtual Ray ScreenToRay(const glm::vec3& pixelPosition) const = 0;
};

class OrthogonalCamera : Camera
{
    const glm::vec3& DIRECTION = { 0, 0, -1 };

public:
    // Construit un raycast à partir de la position d'un pixel
    Ray ScreenToRay(const glm::vec3& pixelPosition) const override;
};

class PerspectiveCamera : Camera
{
    glm::vec3 origin;

public:

    PerspectiveCamera(glm::vec3 newOrigin) : origin(newOrigin)
    {}

    Ray ScreenToRay(const glm::vec3& pixelPosition) const override;
};
