#pragma once

inline bool IsFloatEqual(const float a, const float b, const float epsilon = 0.005f)
{
	return fabs(a - b) < epsilon;
}