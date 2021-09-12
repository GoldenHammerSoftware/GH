// Copyright Golden Hammer Software
#include "GHIntersect.h"
#include "GHMath.h"
#include "math.h"
#include "GHFloat.h"
#include "GHSphere.h"
#include <cfloat>

inline bool validateT(float t)
{
	return t >= 0 && t <= 1.f;
}

float GHIntersect::areaOfTriangle(const GHPoint2& p1,
                                  const GHPoint2& p2,
                                  const GHPoint2& p3)
{
    // top version is an optimization of bottom version.
    // left in bottom version for clarity.
    return .5f * ((p2[0]-p1[0]) * (p3[1]-p1[1]) - (p2[1]-p1[1]) * (p3[0]-p1[0]));
    /*
    GHPoint2 toP2 = p2, toP3 = p3;
    toP2 -= p1;
    toP3 -= p1;
    
    return .5f * (toP2[0]*toP3[1] - toP2[1]*toP3[0]);
     */
}

bool GHIntersect::intersectTri(const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3,
								const GHPoint3& origin, const GHPoint3& dir, float* intersectDist) 
{ 
	// code modified from http://softsurfer.com/Archive/algorithm_0105/algorithm_0105.htm
	// Copyright 2001, softSurfer (www.softsurfer.com)
	// This code may be freely used and modified for any purpose
	// providing that this copyright notice is included with it.
	// SoftSurfer makes no warranty for this code, and cannot be held
	// liable for any real or imagined damage resulting from its use.
	// Users of this code must verify correctness for their application.
	
	GHPoint3 I;		// intersect point
	GHPoint3 u, v, n;  // triangle vectors
    GHPoint3 w0, w;	// ray vectors
    float r, a, b;      // params to calc ray-plane intersect
	
    // get triangle edge vectors and plane normal
	p2.minus(p1, u);
	p3.minus(p1, v);
	GHMath::cross(u, v, n);
	
	if (n[0] == 0 && n[1] == 0 && n[2] == 0) {
		// degenerate triangle
		return false;
	}
	
	origin.minus(p1, w0);
    a = -(n*w0);
    b = (n * dir);
    if (fabs(b) < 0.000001) {     // ray is parallel to triangle plane
        if (a == 0)                // ray lies in triangle plane
            return false;
        else return false;             // ray disjoint from plane
    }
	
    // get intersect point of ray with triangle plane
    r = a / b;
    if (r < 0.0)                   // ray goes away from triangle
        return false;                  // => no intersect
    // for a segment, also test if (r > 1.0) => no intersect
	
	// intersect point of ray and plane
    I = dir;
    I *= r;
    I += origin;
	
    // is I inside T?
    float uu, uv, vv, wu, wv, D;
    uu = (u*u);
    uv = (u*v);
    vv = (v*v);
	
    w = I;
    w -= p1;

    wu = (w*u);
    wv = (w*v);
    D = uv * uv - uu * vv;
	if (D == 0) {
        // will go to NAN
        return false;
    }
    
    // get and test parametric coords
    float s, t;
    s = (uv * wv - vv * wu) / D;
    if (s < -0.002 || s > 1.002)        // I is outside T
        return false;
    t = (uv * wu - uu * wv) / D;
    if (t < -0.002 || (s + t) > 1.002)  // I is outside T
        return false;
	
    if (intersectDist)
    {
        // I is in T
        GHPoint3 intersectDiff = I;
        intersectDiff -= origin;
        *intersectDist = intersectDiff.length();   
    }
    return true;	
}

bool GHIntersect::pointInBox(const GHRect<float, 3>& box, const GHPoint3& pt)
{
    for (int i = 0; i < 3; ++i)
    {
        if (pt[i] < box.mMin[i]
            || pt[i] > box.mMax[i])
        {
            return false;
        }
    }
    return true;
}


bool GHIntersect::rayIntersectSphere(const GHPoint3& center, float radius, const GHPoint3& p1, const GHPoint3& p2)
{
	GHPoint3 testPoint;
	GHIntersect::closestPointOnLineSegment(center, p1, p2, testPoint);
	return GHIntersect::pointInSphere(center, radius, testPoint, (float*)0);
}

bool GHIntersect::rayIntersectBox(const GHRect<float, 3>& box, const GHPoint3& p1, const GHPoint3& p2, 
							  float* dist, GHPoint3* normal)
{
	GHPoint3 dir;
	p2.minus(p1, dir);
	
	GHPoint3 corners[8];
	corners[0].setCoords( box.mMin[0], box.mMax[1], box.mMin[2] );
	corners[1].setCoords( box.mMax[0], box.mMax[1], box.mMin[2] );
	corners[2].setCoords( box.mMax[0], box.mMax[1], box.mMax[2] );
	corners[3].setCoords( box.mMin[0], box.mMax[1], box.mMax[2] );
	corners[4].setCoords( box.mMin[0], box.mMin[1], box.mMin[2] );
	corners[5].setCoords( box.mMax[0], box.mMin[1], box.mMin[2] );
	corners[6].setCoords( box.mMax[0], box.mMin[1], box.mMax[2] );
	corners[7].setCoords( box.mMin[0], box.mMin[1], box.mMax[2] );
	
	bool intersection = 0 != collideQuad( p1, dir, corners[0], corners[1], corners[4], corners[5], dist, normal );
	intersection = intersection || collideQuad( p1, dir, corners[1], corners[2], corners[5], corners[6], dist, normal );
	intersection = intersection || collideQuad( p1, dir, corners[2], corners[3], corners[6], corners[7], dist, normal );
	intersection = intersection || collideQuad( p1, dir, corners[3], corners[0], corners[7], corners[4], dist, normal );
	intersection = intersection || collideQuad( p1, dir, corners[3], corners[2], corners[0], corners[1], dist, normal );
	intersection = intersection || collideQuad( p1, dir, corners[4], corners[5], corners[7], corners[6], dist, normal );
	
	return intersection;
}

bool GHIntersect::rayIntersectBox(const GHRect<float, 2>& box, const GHPoint2& p1, const GHPoint2& p2, float* dist, GHPoint2* normal)
{
	GHPoint2 dir;
	p2.minus(p1, dir);
	
	GHPoint2 corners[4];
	corners[0].setCoords(box.mMin[0], box.mMin[1]);
	corners[1].setCoords(box.mMin[0], box.mMax[1]);
	corners[2].setCoords(box.mMax[0], box.mMax[1]);
	corners[3].setCoords(box.mMax[0], box.mMin[1]);

	class SegmentChecker
	{
	public:
		const GHPoint2 mP1, mP2;
		float mDist;
		GHPoint2* mNormal;
		
	public:
		SegmentChecker(const GHPoint2& pt1, const GHPoint2& pt2, GHPoint2* nrml)
		: mP1(pt1), mP2(pt2), mDist(FLT_MAX), mNormal(nrml) { }
		
		bool checkSegment(const GHPoint2& corner1, const GHPoint2& corner2)
		{
			float boxT = 0, rayT = 0;
			bool ret = lineSegmentIntersect(corner1, corner2, mP1, mP2, boxT, rayT);
			if (validateT(boxT) && rayT >= 0 && rayT < mDist) {
				mDist = rayT;
				if (mNormal) {
					GHPoint2 segmentDist = corner2;
					segmentDist -= corner1;
					mNormal->setCoords(-segmentDist[1], segmentDist[0]);
				}
			}
			return ret;
		}
	};
	
	SegmentChecker checker(p1, p2, normal);
	bool ret = checker.checkSegment(corners[0], corners[1]);
	ret |= checker.checkSegment(corners[1], corners[2]);
	ret |= checker.checkSegment(corners[2], corners[3]);
	ret |= checker.checkSegment(corners[3], corners[0]);
	
	if (dist) {
		*dist = checker.mDist;
	}
	if (normal) {
		normal->normalize();
		GHPoint2 ray = p2;
		ray -= p1;
		if ((*normal)*ray > 0) {
			(*normal)*= -1.f;
		}
	}
	
	return ret;
}

bool GHIntersect::triIntersectSphere(const GHPoint3& center, float radius,
								 const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, 
								 GHPoint3* colPt, float* penetration)
{
	// first find the closest point on the triangle to the sphere.
	GHPoint3 midPoint;
	closestPointOnTri(p1, p2, p3, center, midPoint);

	// then see how close that point is.
	if (colPt) {
		colPt->setCoords(midPoint.getCoords());
	}
	return GHIntersect::pointInSphere(center, radius, midPoint, penetration);
}

bool GHIntersect::elipseIntersectTri(const GHPoint3& center1, const GHPoint3& center2, float radius,
								 const GHPoint3& p1, const GHPoint3& p2, const GHPoint3& p3, 
								 GHPoint3* colPt, float* penetration)
{
	// find the centerish point on the triangle.
    GHPoint3 ray1Diff = p2;
    ray1Diff -= p1;
    
    GHPoint3 ray1Center = ray1Diff;
    ray1Center /= 2.0f;
    ray1Center += p1;
    
	GHPoint3 ray2Diff = p3;
    ray2Diff -= p1;

	GHPoint3 ray2Center = ray2Diff;
    ray2Center /= 2.0;
    ray2Center += p1;

	GHPoint3 centerDiff = ray2Center;
    centerDiff -= ray1Center;
    
	GHPoint3 triCenter = centerDiff;
    triCenter /= 2.0;
    triCenter += ray1Center;
	
	// find the closest point on our elipse to that center point.
	GHPoint3 sphereCenter;
	GHIntersect::closestPointOnLineSegment(triCenter, center1, center2, sphereCenter);
	
	// and then we are left with sphere collision.
	if (colPt) {
		colPt->setCoords(sphereCenter.getCoords());
	}
	return GHIntersect::triIntersectSphere(sphereCenter, radius, p1, p2, p3, colPt, penetration);
}

unsigned short GHIntersect::collideQuad(const GHPoint3& origin, const GHPoint3& dir, 
									const GHPoint3& ul, const GHPoint3& ur, const GHPoint3& bl, const GHPoint3& br, 
									float* dist, GHPoint3* normal)
{
	if (GHIntersect::intersectTri(bl, ul, ur, origin, dir, dist))
	{
		if (normal) {
			GHMath::calcNormal(bl, ul, ur, *normal);
		}
		return 1;
	}
	if (GHIntersect::intersectTri(bl, ur, br, origin, dir, dist))
	{
		if (normal) {
			GHMath::calcNormal(bl, ur, br, *normal);
		}
		return 2;
	}
	
	return 0;
}

bool GHIntersect::rayIntersectInfiniteVerticalCylinder(const GHPoint3& center, float radius, const GHPoint3& p1, const GHPoint3& p2, GHPoint3& penetrationPoint, float& outRayT, GHPoint3& outNormal)
{
	GHPoint2 center2D(center[0], center[2]), p12D(p1[0], p1[2]), p22D(p2[0], p2[2]);
	float circlePenetration;
	if(GHIntersect::rayIntersectSphere(center2D, radius, p12D, p22D, circlePenetration))
	{		
		GHPoint3 circlePenetrationPt = p2;
		circlePenetrationPt -= p1;
		circlePenetrationPt *= circlePenetration;
		circlePenetrationPt += p1;
		
		if(circlePenetration < 0)
			circlePenetrationPt[1] = p1[1];
		
		penetrationPoint = circlePenetrationPt;
		outRayT = circlePenetration;
		outNormal = circlePenetrationPt;
		outNormal -= center;
		outNormal[1] = 0;
		outNormal.normalize();
		return true;
	}
	return false;
}

bool GHIntersect::verticalLineIntersect(const GHPoint2& p1, float yDiff, const GHPoint2& p2, const GHPoint2& v2, float& outT1, float& outT2)
{
	//degenerate case: vertical line is really a point
	if(GHFloat::isZero(yDiff))
		return false;
	
	//other degenerate case: other line is also vertical (therefore, the lines are parallel and do not intersect)
	if(GHFloat::isZero(v2[0]))
		return false;

	outT2 = (p1[0] - p2[0]) / v2[0];
	
	float yVal = p2[1] + outT2 * v2[1];
	outT1 = (yVal - p1[1]) / yDiff;
	
	return true;
}

bool GHIntersect::lineIntersect(const GHPoint2& p1, const GHPoint2& v1, const GHPoint2& p2, const GHPoint2& v2, float& outT1, float& outT2)
{
	//degenerate case: vertical line
	if(GHFloat::isZero(v1[0]))
		return verticalLineIntersect(p1, v1[1], p2, v2, outT1, outT2);
	
	float divV1x = 1.f / v1[0];
	
	float t2denom = v2[1] - v2[0]*v1[1]*divV1x;

	//degenerate case: parallel lines
	if(GHFloat::isZero(t2denom))
		return false;
	
	outT2 = (p1[1] + (p2[0] - p1[0])*v1[1]*divV1x - p2[1]) / t2denom;

	outT1 = (p2[0] + outT2*v2[0] - p1[0])*divV1x;
	
	return true;
}

bool GHIntersect::lineSegmentIntersect(const GHPoint2& start1, const GHPoint2& end1, const GHPoint2& start2, const GHPoint2& end2, float& outT1, float& outT2)
{
	GHPoint2 v1 = end1;
	v1 -= start1;
	
	GHPoint2 v2 = end2;
	v2 -= start2;
	
	if(lineIntersect(start1, v1, start2, v2, outT1, outT2))
	{
		return validateT(outT1) && validateT(outT2);
	}
	return false;
}

// https://www.cs.princeton.edu/courses/archive/fall00/cs426/lectures/raycast/sld017.htm
bool GHIntersect::rayIntersectPlane(const GHPoint3& origin, const GHPoint3& dir, GHPoint4& plane, float& intersectDist)
{
	GHPoint3 planeNormal(plane[0], plane[1], plane[2]);
	float denom = (dir * planeNormal);
	if (GHFloat::isZero(denom))
	{
		intersectDist = 0; // not quite right?
		return false;
	}
	intersectDist = -1.0f * (origin * planeNormal + plane[3]) / denom;
	return (intersectDist > 0);
}

