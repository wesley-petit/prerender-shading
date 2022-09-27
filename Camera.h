#pragma once
#include "Vector3.h"
#include "Ray.h"

class Camera
{
public:
    virtual ~Camera() = default;
    virtual Ray& ScreenToRay(const Vector3& pixelPosition) const = 0;
};

class OrthogonalCamera : Camera
{
    const Vector3& DIRECTION = { 0, 0, -1 };

public:
    // Construit un raycast à partir de la position d'un pixel
    Ray& ScreenToRay(const Vector3& pixelPosition) const override;
};

class PerspectiveCamera : Camera
{
    Vector3 origin;

public:

    PerspectiveCamera(Vector3 newOrigin) : origin(newOrigin)
    {}

    Ray& ScreenToRay(const Vector3& pixelPosition) const override;
};
