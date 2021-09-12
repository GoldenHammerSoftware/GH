// Copyright Golden Hammer Software
#include "GHShadowCameraPositioner.h"
#include "GHMath/GHTransform.h"
#include "GHLightMgr.h"
#include "GHCamera.h"

GHShadowCameraPositioner::GHShadowCameraPositioner(GHCamera& shadowCam, const GHTransform& target,
                                                   const GHPoint3& lightDir)
: mShadowCam(shadowCam)
, mTarget(&target)
, mLightDir(lightDir)
{
}

GHShadowCameraPositioner::GHShadowCameraPositioner(GHCamera& shadowCam, const GHCamera& mainCam,
    const GHPoint3& lightDir, float shadowSize, float targetHeight)
: mShadowCam(shadowCam)
, mMainCam(&mainCam)
, mLightDir(lightDir)
, mShadowSize(shadowSize)
, mTargetHeight(targetHeight)
{
}

void GHShadowCameraPositioner::update(void)
{
    if (mTarget)
    {
        positionAtFixedTarget();
    }
    else
    {
        positionAtCalculatedTarget();
    }
}

void GHShadowCameraPositioner::positionAtFixedTarget(void)
{
    mTarget->getTranslate(mTargetPos);
    positionAtTarget(mTargetPos);
}

void GHShadowCameraPositioner::positionAtCalculatedTarget(void)
{
    // figure out a good direction for the camera offset.
    GHPoint3 viewDir(0, 0, 1);
    mMainCam->getTransform().multDir(viewDir, viewDir);
    // flatten out the z and normalize.
    viewDir[1] = 0.0f;
    viewDir.normalize();

    // figure out a good offset distance from main cam.
    float offsetDist = mShadowSize * 0.4f;

    // figure out a camera center.
    mMainCam->getTransform().getTranslate(mTargetPos);
    viewDir *= offsetDist;
    mTargetPos += viewDir;

    // set the target height on our offset position.
    mTargetPos[1] = mTargetHeight;

    positionAtTarget(mTargetPos);
}

void GHShadowCameraPositioner::positionAtTarget(const GHPoint3& targetPos)
{
    GHPoint3 camOffset(mLightDir);
    // we want to move the camera back along the light dir by an amount that makes sense for the far clip plane.
    // This should be something like -1 * cam.FarClip()/2.
    float offset = mShadowCam.getFarClip();
    const float sCamDist = -1.0f * (offset / 2.0f);
    camOffset *= sCamDist;

    GHPoint3 camPos(targetPos);
    camPos += camOffset;

    // Passing 0,0,1 as the up vector because things go badly if the look dir and up dir are similar.
    // Lights are more likely to look straight down than straight right.  Kinda a bad assumption.
    mShadowCam.getTransform().lookAt(camPos[0], camPos[1], camPos[2],
        targetPos[0], targetPos[1], targetPos[2],
        0, 0, 1);
}
