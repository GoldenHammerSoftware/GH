// Copyright Golden Hammer Software
#pragma once

#include <assert.h>
#include <cmath>
#include "GHMathInclude.h"

// Templated array of type T and length COUNT.
//  Replaces point2, point3, point4 etc.
template< typename T, size_t COUNT >
class GHPoint
{
public:
	GHPoint(void) {}
	GHPoint(const GHPoint<T, COUNT>& other) { setCoords(other.getCoords()); }
	GHPoint(const T* other) { setCoords(other); }
	
	__inline const T* getCoords(void) const { return mCoords; }
    __inline T* getArr(void) { return mCoords; }
	__inline void setCoords(const T* coords);
        
	// some convenience funcs for common lengths.
	// don't pass more arguments than COUNT
	GHPoint(const T& x, const T& y) { setCoords(x,y); }
	GHPoint(const T& x, const T& y, const T& z) { setCoords(x,y,z); }
	GHPoint(const T& x, const T& y, const T& z, const T& w) { setCoords(x,y,z,w); }
	void setCoords(const T& x, const T& y) 
    {
        GHSTATICASSERT(COUNT >= 2);
        mCoords[0] = x; mCoords[1] = y; 
    }
	void setCoords(const T& x, const T& y, const T& z) 
    { 
        GHSTATICASSERT(COUNT >= 3);
        mCoords[0] = x; mCoords[1] = y; mCoords[2] = z; 
    }
	void setCoords(const T& x, const T& y, const T& z, const T& w) 
    {
        GHSTATICASSERT(COUNT >= 4);
        mCoords[0] = x; mCoords[1] = y; mCoords[2] = z; mCoords[3] = w; 
    }
    
	__inline T& operator[](size_t i);
	__inline const T& operator[](size_t i) const;
	
	__inline bool operator==(const GHPoint<T,COUNT>& other) const;
	__inline void operator=(const GHPoint<T,COUNT>& other);
	__inline bool operator<(const GHPoint<T, COUNT>& other) const;

	__inline GHPoint<T,COUNT>& operator+=(const GHPoint<T,COUNT>& other);
	__inline GHPoint<T,COUNT>& operator-=(const GHPoint<T,COUNT>& other);
	__inline GHPoint<T,COUNT>& operator*=(const GHPoint<T,COUNT>& other);
	__inline GHPoint<T,COUNT>& operator/=(const GHPoint<T,COUNT>& other);
	__inline GHPoint<T,COUNT>& operator*=(const T& other);
	__inline GHPoint<T,COUNT>& operator/=(const T& other);
	__inline GHPoint<T,COUNT>& operator+=(const T& other);
	__inline GHPoint<T,COUNT>& operator-=(const T& other);
	__inline float operator*(const GHPoint<T,COUNT>& other) const; // dot
    
	// replacement funcs for +,-,*,/ to avoid accidentally creating new unecessary copies.
	__inline void plus(const GHPoint<T,COUNT>& second, GHPoint<T,COUNT>& ret) const;
	__inline void minus(const GHPoint<T,COUNT>& second, GHPoint<T,COUNT>& ret) const;
	__inline void mult(const T& other, GHPoint<T,COUNT>& ret) const;
	__inline void div(const T& other, GHPoint<T,COUNT>& ret) const;
	__inline void clamp(const GHPoint<T, COUNT>& min, const GHPoint<T, COUNT>& max);

	float normalize(void);
	void linearInterpolate(const GHPoint<T, COUNT>& other, float tVal, GHPoint<T, COUNT>& ret) const;
	float length(void) const;
	float lenSqr(void) const;
	// todo: cross.  There's a GHMath::cross(GHPoint3,GHPoint3) function.

	// copy our coords into a buffer.
	__inline void fillBuffer(T* buffer) const;
	
private:
	// Could pool this memory if we wanted.
	T mCoords[COUNT];
};

#include "GHPoint.inl"

typedef GHPoint<float, 2> GHPoint2;
typedef GHPoint<float, 3> GHPoint3;
typedef GHPoint<float, 4> GHPoint4;
typedef GHPoint<float, 16> GHPoint16;
typedef GHPoint<int, 2> GHPoint2i;
typedef GHPoint<int, 3> GHPoint3i;
