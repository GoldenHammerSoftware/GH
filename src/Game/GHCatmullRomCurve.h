// Copyright Golden Hammer Software
#pragma once

#include "GHSpline.h"

// reference: http://www.mvps.org/directx/articles/catmull/
class GHCatmullRomCurve : public GHSpline
{
public:
	GHCatmullRomCurve(void);

	virtual void addSegPos(const GHPoint3& center, const GHPoint3& leftOffset, const GHPoint3& rightOffset);
	virtual void finalize(void);
	virtual void clear(void);
	virtual float getTotalLength(void) const;
	virtual void getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset,
		unsigned int& indexHint, float& distanceHint) const;
	virtual float getPctForAxisDist(float dist, unsigned short axis, unsigned int indexHint, float distHint) const;

private:
	struct TrailPoint
	{
		GHPoint3 mCenter;
		GHPoint3 mLeftOffset;
		GHPoint3 mRightOffset;
		float mDistFromPrev{ 0 };
		float mTotDist{ 0 };
	};
	// the centerpoints of the nodes.
	// these are used as control points in bezier.
	std::vector<TrailPoint> mPoints;

	// total dist along the center points.
	float mTotalLength;
};
