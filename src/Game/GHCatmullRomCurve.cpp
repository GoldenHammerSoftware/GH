// Copyright Golden Hammer Software
#include "GHCatmullRomCurve.h"
#include "GHPlatform/GHDebugMessage.h"

GHCatmullRomCurve::GHCatmullRomCurve(void)
: mTotalLength(0)
{

}

void GHCatmullRomCurve::clear(void)
{
	mPoints.resize(0);
	mTotalLength = 0;
}

void GHCatmullRomCurve::addSegPos(const GHPoint3& center, const GHPoint3& leftOffset, const GHPoint3& rightOffset)
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

void GHCatmullRomCurve::finalize(void)
{
	// nothing to do

	// verify our data.
	float totLen = 0.0f;
	for (size_t i = 0; i < mPoints.size(); ++i)
	{
		if (i == 0) {
			assert(mPoints[i].mDistFromPrev == 0.0f);
			assert(mPoints[i].mTotDist == 0.0f);
		}
		else {
			GHPoint3 diff(mPoints[i].mCenter);
			diff -= mPoints[i - 1].mCenter;
			float dist = diff.length();
			assert(dist == mPoints[i].mDistFromPrev);
			totLen += dist;
			assert(totLen == mPoints[i].mTotDist);
		}
	}
}

float GHCatmullRomCurve::getTotalLength(void) const
{
	return mTotalLength;
}

float GHCatmullRomCurve::getPctForAxisDist(float dist, unsigned short axis, unsigned int startIndex, float startDist) const
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

void GHCatmullRomCurve::getSegAtPct(float pct, GHPoint3& center, GHPoint3& leftOffset, GHPoint3& rightOffset,
	unsigned int& inOutStartIndex, float& inOutDistAtStartIdx) const
{
	if (!mPoints.size()) {
		return;
	}

	// special case 0.
	if (pct == 0.0f) {
		center = mPoints[0].mCenter;
		leftOffset = mPoints[0].mLeftOffset;
		rightOffset = mPoints[0].mRightOffset;
		return;
	}

	// convert the length into a distance along the path.
	float totLen = getTotalLength();
	float distAtPct = totLen * pct;
	float dist = inOutDistAtStartIdx;

	// find the index of the point that happens just after our point.
	int highIdx = -1;
	float pctUpper = 0.0f;
	for (size_t i = inOutStartIndex; i < mPoints.size(); ++i)
	{
		if (distAtPct <= dist + mPoints[i].mDistFromPrev)
		{
			highIdx = (int)i;
			pctUpper = (distAtPct - dist) / mPoints[i].mDistFromPrev;
			assert(pctUpper >= 0.0f);
			assert(pctUpper <= 1.001f);

			if (i > 0)
			{
				mPoints[i - 1].mLeftOffset.linearInterpolate(mPoints[i].mLeftOffset, pctUpper, leftOffset);
				mPoints[i - 1].mRightOffset.linearInterpolate(mPoints[i].mRightOffset, pctUpper, rightOffset);
				inOutStartIndex = (unsigned int)i - 1;
				inOutDistAtStartIdx = dist - mPoints[i - 1].mDistFromPrev;
			}
			break;
		}
		dist += mPoints[i].mDistFromPrev;
	}

	if (highIdx == -1)
	{
		// we happen after the end of the list.
		leftOffset = mPoints[mPoints.size() - 1].mLeftOffset;
		rightOffset = mPoints[mPoints.size() - 1].mRightOffset;
		center = mPoints[mPoints.size() - 1].mCenter;
		return;
	}
	if (highIdx == 0)
	{
		// we happen before the start of the list.
		leftOffset = mPoints[0].mLeftOffset;
		rightOffset = mPoints[0].mRightOffset;
		center = mPoints[0].mCenter;
		return;
	}
	if (highIdx == 1 || highIdx == mPoints.size()-1)
	{
		// special case the first and last node.  
		// catmullran requires a point before the current segment and a point after.
		mPoints[highIdx-1].mCenter.linearInterpolate(mPoints[highIdx].mCenter, pctUpper, center);
		return;
	}

	// we should have a point before and after our segment, so time to catmull ran.
	/*
	q(t) = 0.5 *(  	(2 * P1) +
 	(-P0 + P2) * t +
	(2*P0 - 5*P1 + 4*P2 - P3) * t2 +
	(-P0 + 3*P1- 3*P2 + P3) * t3)
	*/
	// 0.5 *(  	(2 * P1) +
	GHPoint3 cr1(mPoints[highIdx - 1].mCenter);
	cr1 *= 2.0f;

	// (-P0 + P2) * t +
	GHPoint3 cr2(mPoints[highIdx].mCenter);
	cr2 -= mPoints[highIdx - 2].mCenter;
	cr2 *= pctUpper;

	// (2 * P0 - 5 * P1 + 4 * P2 - P3) * t2 +
	GHPoint3 cr3_1(mPoints[highIdx - 2].mCenter);
	cr3_1 *= 2.0f;
	GHPoint3 cr3_2(mPoints[highIdx - 1].mCenter);
	cr3_2 *= 5.0f;
	GHPoint3 cr3_3(mPoints[highIdx].mCenter);
	cr3_3 *= 4.0f;
	GHPoint3 cr3(cr3_1);
	cr3 -= cr3_2;
	cr3 += cr3_3;
	cr3 -= mPoints[highIdx + 1].mCenter;
	cr3 *= (pctUpper*pctUpper);

	// (-P0 + 3 * P1 - 3 * P2 + P3) * t3)
	GHPoint3 cr4_1(mPoints[highIdx - 1].mCenter);
	cr4_1 *= 3.0f;
	GHPoint3 cr4_2(mPoints[highIdx].mCenter);
	cr4_2 *= 3.0f;
	GHPoint3 cr4(mPoints[highIdx - 2].mCenter);
	cr4 *= -1.0f;
	cr4 += cr4_1;
	cr4 -= cr4_2;
	cr4 += mPoints[highIdx + 1].mCenter;
	cr4 *= (pctUpper*pctUpper*pctUpper);

	center = cr1;
	center += cr2;
	center += cr3;
	center += cr4;
	center *= 0.5f;
}
