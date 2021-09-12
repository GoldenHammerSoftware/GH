// Copyright Golden Hammer Software
#pragma once

#include "GHEpsilon.h"
#include "GHPoint.h"
#include <math.h>
#include <cstdio>

namespace GHFloat
{
	inline bool isEqual(float lhs, float rhs, float eps = EPSILON) 
    {
		float testFloat = (float)::fabs(lhs-rhs);
		return !(testFloat > eps);
	}
    
	inline bool isZero(float f, float eps=EPSILON) 
    {
		return isEqual(f, 0, eps);
	}
	
	inline bool isLessThan(float lhs, float rhs, float eps = EPSILON)
	{
		if (fabs(lhs - rhs) < eps)
		{
			return false;
		}
		return lhs < rhs;
	}

	inline float lerp(float lowerBound, float upperBound, float t) 
    {
		return lowerBound + (upperBound - lowerBound) * t;
	}
	
	inline float sign(float val) 
    {
		return isZero(val) ? 0 : (val > 0 ? 1.f : -1.f);  
	}

    inline bool isNaN(float val)
    {
        volatile float f = val;
        return !(f == f);
    }

	inline int countCharacters(float val)
	{
		char dummy[1];
		return snprintf(dummy, sizeof(dummy), "%f", val);
	}
    
    template <size_t COUNT>
    inline bool isEqual(const GHPoint<float, COUNT>& first, 
                        const GHPoint<float, COUNT>& second,
                        float eps = EPSILON)
    {
        for (unsigned int i = 0; i < COUNT; ++i) 
        {
            if (!isEqual(first[i], second[i], eps)) 
            {
                return false;
            }
        }
        return true;
    }
    
    template <size_t COUNT>
    inline bool isZero(const GHPoint<float, COUNT>& point, float eps=EPSILON)
    {
        for (unsigned int i = 0; i < COUNT; ++i) 
        {
            if (!isZero(point[i], eps)) 
            {
                return false;
            }
        }
        return true;
    }
    
    template <size_t COUNT>
    inline bool isNaN(const GHPoint<float, COUNT>& point)
    {
        for (unsigned int i = 0; i < COUNT; ++i)
        {
            if (isNaN(point[i]))
            {
                return true; 
            }
        }
        return false;
    }
}
