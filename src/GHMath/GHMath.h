// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"
#include "GHFloat.h"

// dumping ground for math funcs without a good home.
class GHMath
{
public:
    template<typename T>
    static T min(const T& left, const T& right);
    template<typename T>
    static T max(const T& left, const T& right);
    template<typename T>
    static T capIncrement(const T& initial, const T& add, const T& max);
	template<typename T>
	static T clamp(const T& min, const T& max, const T& val);
    
   	static void cross(const GHPoint3& first, const GHPoint3& second, GHPoint3& ret);
    static void calcNormal(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, GHPoint3& ret);
    static void calcReflectionVector(const GHPoint3& incident, const GHPoint3& normal, GHPoint3& ret);

   	static float deg2Rad(float angleInDegrees);
    
    static float calcDeterminant3x3(GHPoint<float, 9>& mat);

	// compare two radian values for distance, taking into account wrapping around the circle.
	static float distRadians(float rad1, float rad2);

	template<typename T>
	static float sign(T val);

	template<typename T>
	static T convertRange(T inVal, T minInVal, T maxInVal, T minOutVal, T maxOutVal);
    
/* // does not compile on some platforms (mac, android)
	template<>
	static float sign(float val);
*/

    static void transposeMatrix4(const float* source, float* dest);
    static void transposeMatrix3(const float* source, float* dest);
};

template<typename T>
T GHMath::min(const T& left, const T& right)
{
    if (left < right) return left;
    return right;
}

template<typename T>
T GHMath::max(const T& left, const T& right)
{
    if (left > right) return left;
    return right;
}

template<typename T>
T GHMath::clamp(const T& min, const T& max, const T& val)
{
	return GHMath::max(min, GHMath::min(max, val));
}

template<typename T>
T GHMath::capIncrement(const T& initial, const T& add, const T& max)
{
    T test = initial + add;
    return min<T>(test, max);
}

template<typename T>
float GHMath::sign(T val)
{
	return val == 0.0f ? 0.0f : (val > 0) ? 1.0f : -1.0f;
}

/*
template<>
float GHMath::sign(float val)
{
	return GHFloat::sign(val);
}
*/

template<typename T>
T GHMath::convertRange(T inVal, T minInVal, T maxInVal, T minOutVal, T maxOutVal)
{
	T outVal = inVal;
	outVal -= minInVal;
	outVal /= (maxInVal - minInVal);
	outVal *= (maxOutVal - minOutVal);
	outVal += minOutVal;
	return outVal;
}
