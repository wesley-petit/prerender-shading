#pragma once
#include <cstdlib>

inline bool floatEquals(const float a, const float b, const float epsilon = 0.005f)
{
	return abs(a - b) < epsilon;
}