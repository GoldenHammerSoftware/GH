// Copyright Golden Hammer Software
#include "GHPlane.h"

float GHPlane::distFromPlane(const GHPoint4& plane, const GHPoint3& point)
{
	float ret = 0;
	for (unsigned int i = 0; i < 3; ++i) 
    {
		ret += plane[i] * point[i];
	}
	return ret + plane[3];
}

void GHPlane::normalizeAsPlane(GHPoint4& plane)
{
	float denom = 1.0f / (float)sqrt((plane[0]*plane[0]) + (plane[1]*plane[1]) + (plane[2]*plane[2]));
	for (int i = 0; i < 4; ++i) 
    {
		plane[i] *= denom;
    }
}

