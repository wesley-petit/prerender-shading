#pragma once
#include<cmath>
#include<iostream>

struct Vector3
{
    float x, y, z;

    void Debug() const
    {
        std::cout << "(" << x << ", " << y << ", " << z << ")";
    }

    Vector3 operator*(const float f) const
    {
        return Vector3{ x * f, y * f, z * f };
    }

    Vector3 operator/(const float f) const
    {
        return Vector3{ x / f, y / f, z / f };
    }

    Vector3 operator*(const Vector3 v) const
    {
        return Vector3{ x * v.x, y * v.y, z * v.z };
    }

    Vector3 operator+(const Vector3 v) const
    {
        return Vector3{ x + v.x, y + v.y, z + v.z };
    }

    // Correspond à xb - xa, yb - ya, zb - za
    Vector3 operator-(const Vector3 b) const
    {
        return Vector3{ b.x - x, b.y - y, b.z - z };
    }

    float normSquared() const
    {
        return x * x + y * y + z * z;
    }

    Vector3 unitVector() const
    {
        const float norm = std::sqrt(normSquared());
        return Vector3{ x / norm, y / norm, z / norm };
    }

    float dot(const Vector3 b) const
    {
        return x * b.x + y * b.y + z * b.z;
    }
};

inline Vector3 operator*(const float f, const Vector3 v)
{
    return Vector3{ f * v.x, f * v.y, f * v.z };
}

