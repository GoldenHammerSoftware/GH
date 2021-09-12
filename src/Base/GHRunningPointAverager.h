// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHPlatform/GHTimeVal.h"
#include <list>
#include "GHMath/GHFloat.h"

// keeps a list of GHPoints that expire over time, gives you the current running average.
template< typename T, size_t COUNT >
class GHRunningPointAverager
{
private:
	struct Entry
	{
		GHPoint<T, COUNT> mPoint;
		float mTime;
		float mTimeDelta;
	};

public:
	GHRunningPointAverager(const GHTimeVal& time, float pointLifetime)
		: mTime(time), mPointLifetime(pointLifetime)
	{}

	void addPoint(const GHPoint<T, COUNT>& point)
	{
		// first prune out any expired points.
		while (mPoints.size() && mTime.getTime() - (*mPoints.begin()).mTime > mPointLifetime)
		{
			mPoints.pop_front();
		}
		Entry entry;
		entry.mPoint = point;
		entry.mTime = mTime.getTime();
		entry.mTimeDelta = mTime.getTimePassed();
		mPoints.push_back(entry);
	}

	GHPoint<T, COUNT> getAve(void) const
	{
		GHPoint<T, COUNT> ret;
		for (size_t i = 0; i < COUNT; ++i)
		{
			ret[i] = 0;
		}
		if (!mPoints.size()) {
			return ret;
		}
		typename std::list<Entry>::const_iterator listIter;
		for (listIter = mPoints.begin(); listIter != mPoints.end(); ++listIter)
		{
			ret += (*listIter).mPoint;
		}
		ret /= (float)mPoints.size();
		return ret;
	}

	GHPoint<T, COUNT> getWeightedAve(void) const
	{
		GHPoint<T, COUNT> ret;
		for (size_t i = 0; i < COUNT; ++i)
		{
			ret[i] = 0;
		}
		if (!mPoints.size()) {
			return ret;
		}
		float timeDiff = 0;
		typename std::list<Entry>::const_iterator listIter;
		for (listIter = mPoints.begin(); listIter != mPoints.end(); ++listIter)
		{
			GHPoint<T, COUNT> val = (*listIter).mPoint;
			float delta = (*listIter).mTimeDelta;
			val *= delta;
			ret += val;
			timeDiff += delta;
		}
		if (GHFloat::isZero(timeDiff))
		{
			ret /= (float)mPoints.size();
		}
		else
		{
			ret /= timeDiff;
		}		
		return ret;
	}

	void resetAve(void)
	{
		mPoints.clear();
	}

private:
	const GHTimeVal& mTime;
	float mPointLifetime;
	std::list<Entry> mPoints;
};
