// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHVisType.h"

class GHFrustum
{
public:
	typedef enum {
		CP_NEAR = 0, // must be 0
		CP_LEFT,
		CP_RIGHT,
		CP_TOP,
		CP_BOTTOM,
		CP_MAX
	} CullPlane;

public:
    GHFrustum(void);
    GHFrustum(const GHFrustum& other);
    
	void createPlanes(const GHPoint16& viewProj, float farDist);
	const GHPoint4& getPlane(const CullPlane& plane) const { return mPlanes[plane]; }

	GHVisType::VisType checkVisible(const GHPoint3& center, float radius) const;
	GHVisType::VisType checkVisible(const GHPoint3& min, const GHPoint3& max) const;
	GHVisType::VisType checkVisible(const GHPoint3& point) const;
    // a minor optimization that only looks for partial visibility, never full.
	GHVisType::VisType checkPartialVisible(const GHPoint3& min, const GHPoint3& max) const;

    // we seem to have an issue where orthof causes near and far to be reversed from createPlanes
    void hackForOrthofView(void);
    
    // ways to override far clip
    float getFarDist(void) const { return mFarDist; }
    void setFarDist(float val) { mFarDist = val; }
    
public:
    // hack to disable all culling.
    static bool sAllVisible;
    
private:
	GHVisType::VisType checkPairVisible(const GHPoint3& min, const GHPoint3& max, 
                                        const GHPoint4& plane1, const GHPoint4& plane2) const;
	GHVisType::VisType checkPairPartialVisible(const GHPoint3& min, const GHPoint3& max, 
                                               const GHPoint4& plane1, const GHPoint4& plane2) const;
    
	bool checkPointInsidePlane(const GHPoint3& point, const GHPoint4& plane, float offset=0) const;
    
    // plane 2 is implied by being far dist away from plane1
    GHVisType::VisType checkPairVisible(const GHPoint3& min, const GHPoint3& max,
                                        const GHPoint4& plane1, float far) const;
    // plane 2 is implied by being far dist away from plane1
    GHVisType::VisType checkPairPartialVisible(const GHPoint3& min, const GHPoint3& max,
                                               const GHPoint4& plane1, float far) const;

private:
	GHPoint4 mPlanes[CP_MAX];
    float mFarDist;
};
