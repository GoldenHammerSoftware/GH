// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"

template< typename T, size_t COUNT >
class GHRect
{
public:
    GHRect(void) {}
	GHRect(const GHPoint<T, COUNT>& min, const GHPoint<T, COUNT>& max) : mMin(min), mMax(max) {}
	GHRect(const T* srcBuf);
    
    template<size_t POINTCOUNT>
	bool containsPoint(const GHPoint<T, POINTCOUNT>& pt) const;
	template<size_t POINTCOUNT>
	bool containsPoint(const GHPoint<T, POINTCOUNT>& pt, const T& epsilon) const;
	// take a point and expand our box to fit.
	void checkExpandBounds(const GHPoint<T, COUNT>& point);
	
	void calculateDimensions(GHPoint<T, COUNT>& dim) const;

	void calculateCenter(GHPoint<T, COUNT>& center) const;
	
public:
	// upper left corner.
	GHPoint<T, COUNT> mMin;
	// lower right corner.
	GHPoint<T, COUNT> mMax;
};

template <typename T, size_t COUNT>
GHRect<T, COUNT>::GHRect(const T* srcBuf)
{
    mMin.setCoords(srcBuf);
    srcBuf += COUNT;
    mMax.setCoords(srcBuf);
}

template< typename T, size_t COUNT >
template <size_t POINTCOUNT>
inline bool GHRect<T, COUNT>::containsPoint(const GHPoint<T, POINTCOUNT>& pt) const
{
    assert(POINTCOUNT <= COUNT);
	for (unsigned int i = 0; i < POINTCOUNT; ++i)
	{
		if (pt[i] < mMin[i]) return false;
		if (pt[i] > mMax[i]) return false;
	}
	return true;
}

template< typename T, size_t COUNT >
template <size_t POINTCOUNT>
inline bool GHRect<T, COUNT>::containsPoint(const GHPoint<T, POINTCOUNT>& pt, const T& epsilon) const
{
	assert(POINTCOUNT <= COUNT);
	for (unsigned int i = 0; i < POINTCOUNT; ++i)
	{
		if (pt[i] < mMin[i]-epsilon) return false;
		if (pt[i] > mMax[i]+epsilon) return false;
	}
	return true;
}

template <typename T, size_t COUNT>
inline void GHRect<T, COUNT>::checkExpandBounds(const GHPoint<T, COUNT>& point)
{
	for (unsigned int i = 0; i < COUNT; ++i)
	{
		float testVal = point[i];
		if (testVal < mMin[i]) {
			mMin[i] = testVal;
		}
		if (testVal > mMax[i]) {
			mMax[i] = testVal;
		}
	}
}

template <typename T, size_t COUNT>
inline void GHRect<T, COUNT>::calculateDimensions(GHPoint<T, COUNT>& dim) const
{
	dim = mMax;
	dim -= mMin;
}

template <typename T, size_t COUNT>
inline void GHRect<T, COUNT>::calculateCenter(GHPoint<T, COUNT>& center) const
{
	calculateDimensions(center);
	center *= 0.5f;
	center += mMin;
}

