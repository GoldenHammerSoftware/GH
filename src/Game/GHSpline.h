// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHMath/GHPoint.h"

// Interface for various spline implementations.
class GHSpline
{
public:
	virtual ~GHSpline(void) {}

	// add a control point to our list.
	virtual void addSegPos(const GHPoint3& center, const GHPoint3& leftOffset, const GHPoint3& rightOffset) = 0;
	// do any work that happens after we have a complete list of points.
	virtual void finalize(void) = 0;
	// clear out any data leaving us with an empty list.
	virtual void clear(void) = 0;

	// get the total unsplinified distance of our list of points.
	virtual float getTotalLength(void) const = 0;

	// get the interpolated value at a given pct along our path.
	// call getSegAtPct with 0 0 hints,
	void getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset) const
	{
		unsigned int indexHint = 0;
		float distanceHint = 0.0f;
		getSegAtPct(pct, center, leftOffset, rightOffset, indexHint, distanceHint);
	}
	// index hint is the point in our list where we should start looking.
	// distance hint is the distance at that index hint point.
	virtual void getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset,
		unsigned int& indexHint, float& distanceHint) const = 0;

	// given a distance along one axis, find the total line pct.
	// only works when that axis is sequential in our list.
	// index hint is the point in our list where we should start looking.
	// distance hint is the distance at that index hint point.
	virtual float getPctForAxisDist(float dist, unsigned short axis, unsigned int indexHint, float distHint) const = 0;
};
