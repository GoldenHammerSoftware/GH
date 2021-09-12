// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"

// util funcs for dealing with GHPoint4 as a plane representation
// A GHPlane is Normal.x, Normal.y, Normal.z, DistanceFromOrigin.
class GHPlane
{
public:
	static float distFromPlane(const GHPoint4& plane, const GHPoint3& point);
	static void normalizeAsPlane(GHPoint4& plane);
};
