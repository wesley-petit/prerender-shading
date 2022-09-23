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

#pragma region  operators

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

#pragma endregion

    float normSquared() const
    {
        return x * x + y * y + z * z;
    }

    float norm() const
    {
        return std::sqrt(normSquared());
    }

    Vector3 unitVector() const
    {
        const float normVal = norm();
        return Vector3{ x / normVal, y / normVal, z / normVal };
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

