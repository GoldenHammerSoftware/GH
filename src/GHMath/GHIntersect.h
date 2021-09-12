// Copyright Golden Hammer Software
#pragma once

#include "GHPoint.h"
#include "GHRect.h"
#include "GHFloat.h"

class GHSphere;

// dumping ground for intersect funcs
class GHIntersect
{
public:	
	template<typename T, size_t COUNT>
	static float distFromTri(const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, const GHPoint<T, COUNT>& p3,
		const GHPoint<T, COUNT>& test);
	template<typename T, size_t COUNT>
	static void closestPointOnTri(const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, const GHPoint<T, COUNT>& p3,
		const GHPoint<T, COUNT>& test, GHPoint<T, COUNT>& ret);

	static bool intersectTri(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3,
							 const GHPoint3& origin, const GHPoint3& dir, float* intersectDist);
    
    static float areaOfTriangle(const GHPoint2& p1,
                                const GHPoint2& p2,
                                const GHPoint2& p3);
    
    template<typename T, size_t COUNT>
    static bool pointInTriangle(const GHPoint<T, COUNT>& p1,
                                const GHPoint<T, COUNT>& p2,
                                const GHPoint<T, COUNT>& p3,
                                const GHPoint<T, COUNT>& pt,
                                GHPoint3& outBarycentricCoords);
	
	template<typename T, size_t COUNT>
	static bool pointInSphere(const GHPoint<T, COUNT>& center, T radius, const GHPoint<T, COUNT>& pt, T* penetration);
	static bool pointInBox(const GHRect<float, 3>& box, const GHPoint3& pt);
	template<typename T, size_t COUNT>
	static void closestPointOnLine(const GHPoint<T, COUNT>& pt, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, GHPoint<T, COUNT>& ret, float* tVal);
	template<typename T, size_t COUNT>
	static void closestPointOnLineSegment(const GHPoint<T, COUNT>& pt, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, GHPoint<T, COUNT>& ret);
	static bool rayIntersectSphere(const GHPoint3& center, float radius, const GHPoint3& p1, const GHPoint3& p2);
	template<typename T, size_t COUNT>
	static bool rayIntersectSphere(const GHPoint<T, COUNT>& center, T radius, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, T& outRayT);
	static bool rayIntersectInfiniteVerticalCylinder(const GHPoint3& center, float radius, const GHPoint3& p1, const GHPoint3& p2, GHPoint3& penetrationPoint, float& outRayT, GHPoint3& outNormal);
	static bool rayIntersectBox(const GHRect<float, 3>& box, const GHPoint3& p1, const GHPoint3& p2, float* dist, GHPoint3* normal);
	static bool rayIntersectBox(const GHRect<float, 2>& box, const GHPoint2& p1, const GHPoint2& p2, float* dist, GHPoint2* normal);
	static bool triIntersectSphere(const GHPoint3& center, float radius,
								   const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, 
								   GHPoint3* colPt, float* penetration);
	static bool elipseIntersectTri(const GHPoint3& center1, const GHPoint3& center2, float radius,
								   const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, 
								   GHPoint3* colPt, float* penetration);
	// returns 0 for no collide, 1 for upper left, 2 for lower right.
	static unsigned short collideQuad(const GHPoint3& origin, const GHPoint3& dir, 
									  const GHPoint3& ul, const GHPoint3& ur, const GHPoint3& bl, const GHPoint3& br, 
									  float* dist, GHPoint3* normal);
	
	static bool verticalLineIntersect(const GHPoint2& p1, float yDiff, const GHPoint2& p2, const GHPoint2& v2, float& outT1, float& outT2);
	static bool lineIntersect(const GHPoint2& p1, const GHPoint2& v1, const GHPoint2& p2, const GHPoint2& v2, float& outT1, float& outT2); 
	static bool lineSegmentIntersect(const GHPoint2& start1, const GHPoint2& end1, const GHPoint2& start2, const GHPoint2& end2, float& outT1, float& outT2); 

	template<typename T, size_t COUNT>
	static bool circleIntersect(const GHPoint<T, COUNT>& lhsCenter, float lhsRadius, const GHPoint<T, COUNT>& rhsCenter, float rhsRadius);
	template<typename T, size_t COUNT>
	static bool circleIntersect(const GHPoint<T, COUNT>& lhsCenter, float lhsRadius, const GHPoint<T, COUNT>& rhsCenter, float rhsRadius, GHPoint<T, COUNT>& outIntersectPt, float& outPenetration);

    template<typename T, size_t COUNT>
	static bool circleIntersectAABB(const GHPoint<T, COUNT>& circleCenter, float circleRadius, GHRect<T, COUNT> aabb);

	static bool rayIntersectPlane(const GHPoint3& origin, const GHPoint3& dir, GHPoint4& plane, float& intersectDist);
};


#include "GHIntersect.inl"
