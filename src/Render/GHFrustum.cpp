// Copyright Golden Hammer Software
#include "GHFrustum.h"
#include "GHMath/GHPlane.h"

bool GHFrustum::sAllVisible = false;

GHFrustum::GHFrustum(void)
: mFarDist(0)
{
}

GHFrustum::GHFrustum(const GHFrustum& other)
{
    mFarDist = other.mFarDist;
    for (int i = 0; i < CP_MAX; ++i)
    {
        mPlanes[i] = other.mPlanes[i];
    }
}

void GHFrustum::createPlanes(const GHPoint<float, 16>& viewProj, float farDist)
{
    mFarDist = farDist;
    
	mPlanes[CP_LEFT][0] = viewProj.getCoords()[3]+viewProj.getCoords()[0];
	mPlanes[CP_LEFT][1] = viewProj.getCoords()[7]+viewProj.getCoords()[4];
	mPlanes[CP_LEFT][2] = viewProj.getCoords()[11]+viewProj.getCoords()[8];
	mPlanes[CP_LEFT][3] = viewProj.getCoords()[15]+viewProj.getCoords()[12];
    
	mPlanes[CP_RIGHT][0] = viewProj.getCoords()[3]-viewProj.getCoords()[0];
	mPlanes[CP_RIGHT][1] = viewProj.getCoords()[7]-viewProj.getCoords()[4];
	mPlanes[CP_RIGHT][2] = viewProj.getCoords()[11]-viewProj.getCoords()[8];
	mPlanes[CP_RIGHT][3] = viewProj.getCoords()[15]-viewProj.getCoords()[12];
    
	mPlanes[CP_BOTTOM][0] = viewProj.getCoords()[3]+viewProj.getCoords()[1];
	mPlanes[CP_BOTTOM][1] = viewProj.getCoords()[7]+viewProj.getCoords()[5];
	mPlanes[CP_BOTTOM][2] = viewProj.getCoords()[11]+viewProj.getCoords()[9];
	mPlanes[CP_BOTTOM][3] = viewProj.getCoords()[15]+viewProj.getCoords()[13];
    
	mPlanes[CP_TOP][0] = viewProj.getCoords()[3]-viewProj.getCoords()[1];
	mPlanes[CP_TOP][1] = viewProj.getCoords()[7]-viewProj.getCoords()[5];
	mPlanes[CP_TOP][2] = viewProj.getCoords()[11]-viewProj.getCoords()[9];
	mPlanes[CP_TOP][3] = viewProj.getCoords()[15]-viewProj.getCoords()[13];
    
    /*
	mPlanes[CP_FAR][0] = viewProj.getCoords()[3]-viewProj.getCoords()[2];
	mPlanes[CP_FAR][1] = viewProj.getCoords()[7]-viewProj.getCoords()[6];
	mPlanes[CP_FAR][2] = viewProj.getCoords()[11]-viewProj.getCoords()[10];
	mPlanes[CP_FAR][3] = viewProj.getCoords()[15]-viewProj.getCoords()[14];
    */
    
	mPlanes[CP_NEAR][0] = viewProj.getCoords()[2];
	mPlanes[CP_NEAR][1] = viewProj.getCoords()[6];
	mPlanes[CP_NEAR][2] = viewProj.getCoords()[10];
	mPlanes[CP_NEAR][3] = viewProj.getCoords()[14];
    
	for (int i = 0; i < CP_MAX; ++i) {
		GHPlane::normalizeAsPlane(mPlanes[i]);
	}
}

void GHFrustum::hackForOrthofView(void)
{
    mPlanes[CP_NEAR] *= -1;
}

GHVisType::VisType GHFrustum::checkVisible(const GHPoint3& point) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_FULL;

    // near plane serves as far plane also.
	float dist = GHPlane::distFromPlane(mPlanes[CP_NEAR], point);
	if (dist < 0 || dist > mFarDist)
    {
		return GHVisType::VT_CULL;
	}

	for (int i = 1; i < CP_MAX; i++) {
		if (GHPlane::distFromPlane(mPlanes[i], point) < 0) {
			return GHVisType::VT_CULL;
		}
	}
	return GHVisType::VT_FULL;
}

GHVisType::VisType GHFrustum::checkVisible(const GHPoint3& center, float radius) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_FULL;

    // near plane serves as far plane also.
	float dist = GHPlane::distFromPlane(mPlanes[CP_NEAR], center);
	if (dist < -radius || dist > mFarDist+radius)
    {
		return GHVisType::VT_CULL;
	}
	bool partial = false;
	if (dist <= 0 || dist >= mFarDist) {
		partial = true;
	}

	for (int i = 1; i < CP_MAX; i++) {
		dist = GHPlane::distFromPlane(mPlanes[i], center);
		if (dist < -radius) {
			return GHVisType::VT_CULL;
		}
		if (dist <= 0) {
			partial = true;
		}
	}
	if (partial) return GHVisType::VT_PARTIAL;
	return GHVisType::VT_FULL;
}

GHVisType::VisType GHFrustum::checkVisible(const GHPoint3& min, const GHPoint3& max) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_FULL;

	GHVisType::VisType ret = GHVisType::VT_CULL;
	ret = checkPairVisible(min, max, mPlanes[CP_LEFT], mPlanes[CP_RIGHT]);
	if (ret == GHVisType::VT_CULL) {
		return ret;
	}
    
	if (ret == GHVisType::VT_PARTIAL) {
		ret = checkPairPartialVisible(min, max, mPlanes[CP_TOP], mPlanes[CP_BOTTOM]);
	}
	else {
		ret = checkPairVisible(min, max, mPlanes[CP_TOP], mPlanes[CP_BOTTOM]);
	}
	if (ret == GHVisType::VT_CULL) {
		return ret;
	}
    
    if (ret == GHVisType::VT_PARTIAL) {
		ret = checkPairPartialVisible(min, max, mPlanes[CP_NEAR], mFarDist);
	}
	else {
		ret = checkPairVisible(min, max, mPlanes[CP_NEAR], mFarDist);
	}
    
	return ret;
}

GHVisType::VisType GHFrustum::checkPartialVisible(const GHPoint3& min, const GHPoint3& max) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_PARTIAL;

	if (checkPairPartialVisible(min, max, mPlanes[CP_LEFT], mPlanes[CP_RIGHT]) == GHVisType::VT_CULL) {
		return GHVisType::VT_CULL;
	}
	if (checkPairPartialVisible(min, max, mPlanes[CP_TOP], mPlanes[CP_BOTTOM]) == GHVisType::VT_CULL) {
		return GHVisType::VT_CULL;
	}
    
    if (checkPairPartialVisible(min, max, mPlanes[CP_NEAR], mFarDist) == GHVisType::VT_CULL) {
		return GHVisType::VT_CULL;
	}
	
	return GHVisType::VT_PARTIAL;
}

GHVisType::VisType GHFrustum::checkPairVisible(const GHPoint3& min, const GHPoint3& max, 
                                               const GHPoint4& plane1, const GHPoint4& plane2) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_FULL;

	bool plane1HasPointInside = false;
	bool plane2HasPointInside = false;
	bool plane1HasPointOutside = false;
	bool plane2HasPointOutside = false;
    
	GHVisType::VisType ret = GHVisType::VT_CULL;
	bool inside1, inside2;
    GHPoint3 testPoint;

    testPoint[0] = min[0];
    for (int x = 0; x < 2; ++x)
    {
        if (x == 1) testPoint[0] = max[0];
        testPoint[1] = min[1];
        
        for (int y = 0; y < 2; ++y)
        {
            if (y == 1) testPoint[1] = max[1];
            testPoint[2] = min[2];
            
            for (int z = 0; z < 2; ++z)
            {
                if (z == 1) testPoint[2] = max[2];

                inside1 = checkPointInsidePlane(testPoint, plane1);
                plane1HasPointInside |= inside1;
                plane1HasPointOutside |= (!inside1);
                if (plane1HasPointInside && plane1HasPointOutside) {
                    return GHVisType::VT_PARTIAL;
                }
                
                inside2 = checkPointInsidePlane(testPoint, plane2);
                plane2HasPointInside |= inside2;
                plane2HasPointOutside |= (!inside2);
                if (plane2HasPointInside && plane2HasPointOutside) {
                    return GHVisType::VT_PARTIAL;
                }
                
                if (inside1 && inside2) {
                    ret = GHVisType::VT_PARTIAL;
                }
            }
        }
    }

	if (!plane1HasPointOutside && !plane2HasPointOutside) {
		return GHVisType::VT_FULL;
	}
	if (plane1HasPointInside && plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	if (!plane1HasPointInside && !plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	return ret;
}

GHVisType::VisType GHFrustum::checkPairPartialVisible(const GHPoint3& min, const GHPoint3& max, 
                                                      const GHPoint4& plane1, const GHPoint4& plane2) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_PARTIAL;

	bool plane1HasPointInside = false;
	bool plane2HasPointInside = false;
	bool plane1HasPointOutside = false;
	bool plane2HasPointOutside = false;
	bool inside1, inside2;
    
    GHPoint3 testPoint;
    testPoint[0] = min[0];
    for (int x = 0; x < 2; ++x)
    {
        if (x == 1) testPoint[0] = max[0];
        testPoint[1] = min[1];
        
        for (int y = 0; y < 2; ++y)
        {
            if (y == 1) testPoint[1] = max[1];
            testPoint[2] = min[2];
            
            for (int z = 0; z < 2; ++z)
            {
                if (z == 1) testPoint[2] = max[2];

                if (!plane1HasPointInside || !plane1HasPointOutside) {
                    inside1 = checkPointInsidePlane(testPoint, plane1);
                    plane1HasPointInside |= inside1;
                    plane1HasPointOutside |= !inside1;
                    if (plane1HasPointInside && plane1HasPointOutside) {
                        return GHVisType::VT_PARTIAL;
                    }
                }
                
                if (!plane2HasPointInside || !plane2HasPointOutside) {
                    inside2 = checkPointInsidePlane(testPoint, plane2);
                    plane2HasPointInside |= inside2;
                    plane2HasPointOutside |= !inside2;
                    if (plane2HasPointInside && plane2HasPointOutside) {
                        return GHVisType::VT_PARTIAL;
                    }
                }
                
                if (inside1 && inside2) {
                    return GHVisType::VT_PARTIAL;
                }
            }
        }
    }
    
	if (!plane1HasPointOutside && !plane2HasPointOutside) {
		return GHVisType::VT_PARTIAL;
	}
	if (plane1HasPointInside && plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	if (!plane1HasPointInside && !plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	return GHVisType::VT_CULL;
}

bool GHFrustum::checkPointInsidePlane(const GHPoint3& point, const GHPoint4& plane, float offset) const
{
	return (GHPlane::distFromPlane(plane, point) > -offset);
}

GHVisType::VisType GHFrustum::checkPairVisible(const GHPoint3& min, const GHPoint3& max,
                                               const GHPoint4& plane1, float far) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_FULL;
    
	bool plane1HasPointInside = false;
	bool plane2HasPointInside = false;
	bool plane1HasPointOutside = false;
	bool plane2HasPointOutside = false;
    
	GHVisType::VisType ret = GHVisType::VT_CULL;
	bool inside1, inside2;
    GHPoint3 testPoint;
    
    testPoint[0] = min[0];
    for (int x = 0; x < 2; ++x)
    {
        if (x == 1) testPoint[0] = max[0];
        testPoint[1] = min[1];
        
        for (int y = 0; y < 2; ++y)
        {
            if (y == 1) testPoint[1] = max[1];
            testPoint[2] = min[2];
            
            for (int z = 0; z < 2; ++z)
            {
                if (z == 1) testPoint[2] = max[2];
                
                float dist = GHPlane::distFromPlane(plane1, testPoint);
                inside1 = (dist > 0);
                plane1HasPointInside |= inside1;
                plane1HasPointOutside |= (!inside1);
                if (plane1HasPointInside && plane1HasPointOutside) {
                    return GHVisType::VT_PARTIAL;
                }

                inside2 = (dist < far);
                plane2HasPointInside |= inside2;
                plane2HasPointOutside |= (!inside2);
                if (plane2HasPointInside && plane2HasPointOutside) {
                    return GHVisType::VT_PARTIAL;
                }
                
                if (inside1 && inside2) {
                    ret = GHVisType::VT_PARTIAL;
                }
            }
        }
    }
    
	if (!plane1HasPointOutside && !plane2HasPointOutside) {
		return GHVisType::VT_FULL;
	}
	if (plane1HasPointInside && plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	if (!plane1HasPointInside && !plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	return ret;
}

GHVisType::VisType GHFrustum::checkPairPartialVisible(const GHPoint3& min, const GHPoint3& max,
                                                      const GHPoint4& plane1, float far) const
{
    if (GHFrustum::sAllVisible) return GHVisType::VT_PARTIAL;
    
	bool plane1HasPointInside = false;
	bool plane2HasPointInside = false;
	bool plane1HasPointOutside = false;
	bool plane2HasPointOutside = false;
	bool inside1, inside2;
    
    GHPoint3 testPoint;
    testPoint[0] = min[0];
    for (int x = 0; x < 2; ++x)
    {
        if (x == 1) testPoint[0] = max[0];
        testPoint[1] = min[1];
        
        for (int y = 0; y < 2; ++y)
        {
            if (y == 1) testPoint[1] = max[1];
            testPoint[2] = min[2];
            
            for (int z = 0; z < 2; ++z)
            {
                if (z == 1) testPoint[2] = max[2];
                
                float dist = GHPlane::distFromPlane(plane1, testPoint);
                if (!plane1HasPointInside || !plane1HasPointOutside) {
                    inside1 = (dist > 0);
                    plane1HasPointInside |= inside1;
                    plane1HasPointOutside |= !inside1;
                    if (plane1HasPointInside && plane1HasPointOutside) {
                        return GHVisType::VT_PARTIAL;
                    }
                }
                
                if (!plane2HasPointInside || !plane2HasPointOutside) {
                    inside2 = (dist < far);
                    plane2HasPointInside |= inside2;
                    plane2HasPointOutside |= !inside2;
                    if (plane2HasPointInside && plane2HasPointOutside) {
                        return GHVisType::VT_PARTIAL;
                    }
                }
                
                if (inside1 && inside2) {
                    return GHVisType::VT_PARTIAL;
                }
            }
        }
    }
    
	if (!plane1HasPointOutside && !plane2HasPointOutside) {
		return GHVisType::VT_PARTIAL;
	}
	if (plane1HasPointInside && plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	if (!plane1HasPointInside && !plane2HasPointInside) {
		return GHVisType::VT_PARTIAL;
	}
	return GHVisType::VT_CULL;
}

