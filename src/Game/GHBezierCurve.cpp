// Copyright Golden Hammer Software
#include "GHBezierCurve.h"
#include "GHPlatform/GHDebugMessage.h"

GHBezierCurve::GHBezierCurve(void)
	: mTotalLength(0)
{

}

void GHBezierCurve::clear(void)
{
	mPoints.resize(0);
	mBezierPoints.resize(0);
	mTotalLength = 0;
}

void GHBezierCurve::addSegPos(const GHPoint3& center, const GHPoint3& leftOffset, const GHPoint3& rightOffset)
{
	TrailPoint newPoint;
	newPoint.mCenter = center;
	newPoint.mLeftOffset = leftOffset;
	newPoint.mRightOffset = rightOffset;
	newPoint.mDistFromPrev = 0.0f;
	if (this->mPoints.size())
	{
		const TrailPoint& lastPos = mPoints[mPoints.size() - 1];
		GHPoint3 posDiff = lastPos.mCenter;
		posDiff -= center;
		newPoint.mDistFromPrev = posDiff.length();
	}
	mTotalLength += newPoint.mDistFromPrev;
	newPoint.mTotDist = mTotalLength;
	mPoints.push_back(newPoint);
}

void GHBezierCurve::finalize(void)
{
	createBezierPoints();
}

float GHBezierCurve::getTotalLength(void) const
{
	return mTotalLength;
}

float GHBezierCurve::getPctForAxisDist(float dist, unsigned short axis, unsigned int startIndex, float startDist) const
{
	assert(axis < 3);

	float lineDist = startDist;
	for (size_t i = startIndex; i < mPoints.size(); ++i)
	{
		if (mPoints[i].mCenter[axis] > dist)
		{
			if (i == 0) {
				return 0;
			}
			float prevAxisDist = mPoints[i - 1].mCenter[axis];
			float axisDistFromPrev = dist - prevAxisDist;
			float currSegAxisDist = mPoints[i].mCenter[axis] - prevAxisDist;
			float pctAlongSeg = 0.0f;
			if (currSegAxisDist != 0.0f) 
			{
				pctAlongSeg = axisDistFromPrev / currSegAxisDist;
			}
			lineDist += mPoints[i].mDistFromPrev*pctAlongSeg;
			return lineDist / getTotalLength();
		}
		lineDist += mPoints[i].mDistFromPrev;
	}
	return 1.0f;
}

void GHBezierCurve::getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset, 
	unsigned int& inOutStartIndex, float& inOutDistAtStartIdx) const
{
	if (!mPoints.size() || !mBezierPoints.size()) {
		return;
	}

	// special case 0.
	// also special case a song with only one index point.
	if (pct == 0.0f || mPoints.size() < 2) 
	{
		center = mPoints[0].mCenter;
		leftOffset = mPoints[0].mLeftOffset;
		rightOffset = mPoints[0].mRightOffset;
		return;
	}

	// convert the length into a distance along the path.
	float totLen = getTotalLength();
	float distAtPct = totLen * pct;
	float dist = inOutDistAtStartIdx;
	
	/*
	dist = 0;
	for (size_t i = 0; i < inOutStartIndex; ++i)
	{
		dist += mPoints[i].mDistFromPrev;
	}//*/

	for (size_t i = inOutStartIndex; i < mPoints.size(); ++i)
	{
		if (distAtPct <= dist + mPoints[i].mDistFromPrev)
		{
			assert(i != 0);
			float pctUpper = (distAtPct - dist) / mPoints[i].mDistFromPrev;

			mPoints[i - 1].mLeftOffset.linearInterpolate(mPoints[i].mLeftOffset, pctUpper, leftOffset);
			mPoints[i - 1].mRightOffset.linearInterpolate(mPoints[i].mRightOffset, pctUpper, rightOffset);
			inOutStartIndex = (unsigned int)i - 1;
			inOutDistAtStartIdx = dist - mPoints[i - 1].mDistFromPrev;

			if (i == 1 && pctUpper < 0.5f)
			{
				// if we're below the first bezier point, then do a linear interpolate.
				mPoints[0].mCenter.linearInterpolate(mBezierPoints[0], pctUpper*2.0f, center);
			}
			else if (i == mPoints.size() - 1 && pctUpper >= 0.5f)
			{
				// same with the final bezier point.
				mPoints[i].mCenter.linearInterpolate(mBezierPoints[mBezierPoints.size() - 1], 1.0f - (pctUpper - 0.5f)*2.0f, center);
			}
			else if (mPoints.size() < 3)
			{
				mPoints[i - 1].mCenter.linearInterpolate(mPoints[i].mCenter, pctUpper, center);
			}
			else
			{
				// calc the pctage between the two bezier points.
				///*
				int pointIdx = (int)i;
				if (pctUpper < 0.5f)
				{
					pointIdx = (int)i - 1;
				}
				int lowBezier = pointIdx - 1;
				float lowPointDist = mPoints[pointIdx - 1].mTotDist;
				float highPointDist = mPoints[pointIdx].mTotDist;
				float nextPointDist = mPoints[pointIdx + 1].mTotDist;
				float distAtLower = lowPointDist + (highPointDist - lowPointDist) / 2.0f;
				float distAtHigher = highPointDist + (nextPointDist - highPointDist) / 2.0f;
				assert(distAtLower <= distAtHigher);
				float totSegDist = distAtHigher - distAtLower;

				assert(totSegDist > 0.0f);
				assert(distAtPct >= (distAtLower-0.0001) && distAtPct <= (distAtHigher+0.0001));
				if (distAtLower > distAtPct) distAtLower = distAtPct;
				if (distAtHigher < distAtPct) distAtHigher = distAtPct;
				
				float bezierPct = (distAtPct - distAtLower) / totSegDist;
				
				assert(bezierPct >= 0.0f);
				assert(bezierPct <= 1.0f);
				//GHDebugMessage::outputString("i %d lowBezier %d pctUpper %f BezPct %f", pointIdx, lowBezier, pctUpper, bezierPct);
				GHPoint3 lowPoint, highPoint;
				mBezierPoints[lowBezier].linearInterpolate(mPoints[pointIdx].mCenter, bezierPct, lowPoint);
				mPoints[pointIdx].mCenter.linearInterpolate(mBezierPoints[lowBezier + 1], bezierPct, highPoint);
				lowPoint.linearInterpolate(highPoint, bezierPct, center);
			}

			return;
		}
		dist += mPoints[i].mDistFromPrev;
	}
	GHDebugMessage::outputString("Failed to find point!!!");
}

void GHBezierCurve::createBezierPoints(void)
{
	// the bezier fixed points are the subdivisions of our input segments.
	mBezierPoints.clear();
	for (size_t i = 1; i < mPoints.size(); ++i)
	{
		GHPoint3 point;
		mPoints[i - 1].mCenter.linearInterpolate(mPoints[i].mCenter, 0.5f, point);
		mBezierPoints.push_back(point);
	}
}
