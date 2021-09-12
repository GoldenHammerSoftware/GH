// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHMath/GHPoint.h"

class GHTransform;
class GHCamera;

// positions a camera for casting a shadow on an object
class GHShadowCameraPositioner : public GHController
{
public:
    // Use this constructor if we have a concrete shadow target.
    GHShadowCameraPositioner(GHCamera& shadowCam, const GHTransform& target, const GHPoint3& lightDir);
    // Use this constructor if we want to calculate the shadow target.
    GHShadowCameraPositioner(GHCamera& shadowCam, const GHCamera& mainCam, const GHPoint3& lightDir, 
        float shadowSize, float targetHeight);

    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}

    float getShadowSize(void) const { return mShadowSize; }
    const GHPoint3& getShadowCenter(void) const { return mTargetPos; }

private:
    void positionAtFixedTarget(void);
    void positionAtCalculatedTarget(void);
    void positionAtTarget(const GHPoint3& targetPos);

private:
    GHCamera& mShadowCam;
    const GHPoint3& mLightDir;
    // If not null we use this as the shadow center.
    // Otherwise we calculate shadow center from mainCam and shadowSize.
    const GHTransform* mTarget{ 0 };

    const GHCamera* mMainCam{ 0 };
    // size of the shadow coverage in world units.
    // used if mMainCam is not null to pick a good shadow target.
    float mShadowSize{ 0 };
    float mTargetHeight{ 0 };

    // our target pos.  either calculated from mMainCam or from mTarget
    GHPoint3 mTargetPos;
};
