// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHSpline.h"

// given a list of trail nodes provide a way to interpolate along the path.
class GHBezierCurve : public GHSpline
{
public:
	GHBezierCurve(void);

	virtual void clear(void);
	virtual void addSegPos(const GHPoint3& center, const GHPoint3& leftOffset, const GHPoint3& rightOffset);
	virtual void finalize(void);

	virtual float getTotalLength(void) const;
	virtual void getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset, unsigned int& inOutStartIdx, float& inOutDistAtStartIdx) const;

	// given a distance along one axis, find the total line pct.
	// only works when that axis is sequential in our list.
	virtual float getPctForAxisDist(float dist, unsigned short axis, unsigned int startIndex, float startDist) const;

private:
	void createBezierPoints(void);

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

	// the fixed points used for bezier creation.
	// these are subdivisions of mPoints.
	std::vector<GHPoint3> mBezierPoints;

	float mTotalLength{ 0 };
};
