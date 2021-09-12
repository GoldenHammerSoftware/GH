#pragma once 

#include <algorithm> //for std::min, std::max

template<typename T, size_t COUNT>
bool GHIntersect::pointInTriangle(const GHPoint<T, COUNT>& p1,
                                  const GHPoint<T, COUNT>& p2,
                                  const GHPoint<T, COUNT>& p3,
                                  const GHPoint<T, COUNT>& pt,
                                  GHPoint3& outBarycentricCoords)
{
    float divTriArea = 1.f/areaOfTriangle(p1, p2, p3);
    float area23 = areaOfTriangle(pt, p2, p3);
    float area31 = areaOfTriangle(pt, p3, p1);
    float area12 = areaOfTriangle(pt, p1, p2);
    
    outBarycentricCoords[0] = area23*divTriArea;
    outBarycentricCoords[1] = area31*divTriArea;
    outBarycentricCoords[2] = area12*divTriArea;
    
    return (outBarycentricCoords[0] >= 0
            && outBarycentricCoords[1] >= 0
            && outBarycentricCoords[2] >= 0);
}

template<typename T, size_t COUNT>
bool GHIntersect::pointInSphere(const GHPoint<T, COUNT>& center, T radius, const GHPoint<T, COUNT>& pt, T* penetration)
{
	GHPoint<T, COUNT> diff;
	center.minus(pt, diff);
	float sqrLen = diff.lenSqr();
	if (sqrLen > radius*radius) {
		return false;
	}
	if (penetration) {
		float len = (float)::sqrt(sqrLen);
		*penetration = radius - len;
	}
	return true;
}

template<typename T, size_t COUNT>
bool GHIntersect::rayIntersectSphere(const GHPoint<T, COUNT>& center, T radius, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, T& outRayT)
{
	//Solution using Quadratic Formula. Mostly taken from the book Real-Time Collision Detection, by Christer Ericson
	
	GHPoint<T, COUNT> p = p1;
	p -= center;
	
	GHPoint<T, COUNT> v = p2;
	v -= p1;

	float a = v*v;
	float c = p.lenSqr() - radius*radius;

	//Early out if line segment is nowhere near the sphere 
	if(c > a)
		return false;
	
	//Quadratic formula:
	// t =  (-b (+/-) sqrt(b^2 - 4a*c)) / 2a
	
	//Formula for sphere
	// sum(components*components) = radius*radius
	
	//Insert Formula for line (assume p is wrt sphere center, v is the distance between p1 and p2)
	// sumComponents( (p + t*v)^2 ) = radius*radius
	
	//solve for t
	//sumComponents( t^2(v^2) + t(2*v*p) + p^2 - radius^2 ) = 0
	
	float b = (v*p);
	
	//early out if ray points away from sphere, or is nonexistent
	if((c > 0 && b > 0) || GHFloat::isZero(a))
		return false;
	
	float discriminant = b*b - a*c;
	
	//early out if ray misses sphere entirely
	if(discriminant < 0)
		return false;
	
	discriminant = (float)sqrt(discriminant);
	
	float div2A = 1.f / a;
	float t1 = (-b + discriminant) * div2A;
	float t2 = (-b - discriminant) * div2A;
	
	//out if ray doesn't hit sphere quite yet
	if(t1 > 1.f && t2 > 1.f)
		return false;
	
	
	//Note: if p1 starts out inside the sphere, we may return a negative t value
	// This may or may not be desirable, depending on usage (it is for now).
	outRayT = std::min(t1, t2);
	return true;
}

template<typename T, size_t COUNT>
void GHIntersect::closestPointOnLine(const GHPoint<T, COUNT>& pt, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, GHPoint<T, COUNT>& ret, float* tVal)
{
	// ripped from http://www.flipcode.com/cgi-bin/fcarticles.cgi?show=64047
	GHPoint<T, COUNT> c, V;
	pt.minus(p1, c);
	p2.minus(p1, V);
	float vLength = V.normalize();
	
	float t = (V*c);
    V *= t;
	p1.plus(V, ret);
	
	if (tVal) {
		if (!GHFloat::isZero(vLength)) {
			*tVal = t/vLength;
		}
		else {
			*tVal = t;
		}
	}
}

template<typename T, size_t COUNT>
void GHIntersect::closestPointOnLineSegment(const GHPoint<T, COUNT>& pt, const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, GHPoint<T, COUNT>& ret)
{
	float t;
	closestPointOnLine(pt, p1, p2, ret, &t);
	
	if (t < 0.0f) {
		ret.setCoords(p1.getCoords());
		return;
	}
	if (t > 1.f) {
		ret.setCoords(p2.getCoords());
		return;
	}
}

template<typename T, size_t COUNT>
float GHIntersect::distFromTri(const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, const GHPoint<T, COUNT>& p3,
	const GHPoint<T, COUNT>& test)
{
	GHPoint<T, COUNT> colPos;
	closestPointOnTri(p1, p2, p3, test, colPos);
	colPos -= test;
	return colPos.length();
}

template<typename T, size_t COUNT>
void GHIntersect::closestPointOnTri(const GHPoint<T, COUNT>& p1, const GHPoint<T, COUNT>& p2, const GHPoint<T, COUNT>& p3,
	const GHPoint<T, COUNT>& test, GHPoint<T, COUNT>& ret)
{
	GHPoint<T, COUNT> ray1pt, ray2pt;
	GHIntersect::closestPointOnLineSegment(test, p1, p2, ray1pt);
	GHIntersect::closestPointOnLineSegment(test, p1, p3, ray2pt);
	GHIntersect::closestPointOnLineSegment(test, ray1pt, ray2pt, ret);
}

template<typename T, size_t COUNT>
bool GHIntersect::circleIntersect(const GHPoint<T, COUNT>& lhsCenter, float lhsRadius, const GHPoint<T, COUNT>& rhsCenter, float rhsRadius)
{
	GHPoint<T, COUNT> diff = lhsCenter;
	diff -= rhsCenter;
	float radius = lhsRadius + rhsRadius;
	return diff.lenSqr() < radius*radius;
}

template<typename T, size_t COUNT>
bool GHIntersect::circleIntersect(const GHPoint<T, COUNT>& lhsCenter, float lhsRadius, const GHPoint<T, COUNT>& rhsCenter, float rhsRadius, GHPoint<T, COUNT>& outIntersectPt, float& outPenetration)
{
	if (circleIntersect(lhsCenter, lhsRadius, rhsCenter, rhsRadius)) {
		GHPoint<T, COUNT> diff = lhsCenter;
		diff -= rhsCenter;
		
		float diffLen = diff.length();

		outPenetration = (lhsRadius + rhsRadius - diffLen);
		
		if (!GHFloat::isZero(diffLen)) {
			diff /= diffLen;
		}
		
		outIntersectPt = diff;
		outIntersectPt *= lhsRadius - outPenetration*.5f;
		outIntersectPt += lhsCenter;
		
		return true;
	}
	return false;
}

template<typename T, size_t COUNT>
bool GHIntersect::circleIntersectAABB(const GHPoint<T, COUNT>& circleCenter, float circleRadius, GHRect<T, COUNT> aabb)
{
    for (size_t i = 0; i < COUNT; ++i)
    {
        if (circleCenter[i] + circleRadius < aabb.mMin[i]) return false;
        if (circleCenter[i] - circleRadius > aabb.mMax[i]) return false;
    }
    return true;
}
