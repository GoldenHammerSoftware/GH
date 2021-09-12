// Copyright Golden Hammer Software
#include "GHSphereBounds.h"
#include "GHTransformNode.h"

GHSphereBounds::GHSphereBounds(void)
: mWorldVersion(-1)
, mTransform(0)
{
}

const GHSphere& GHSphereBounds::getSphere(void) const
{
    if (!mTransform) return mLocalSphere;
    if (mWorldVersion != mTransform->getTransform().getVersion()) 
    {
        mTransform->getTransform().mult(mLocalSphere.mCenter, mWorldSphere.mCenter);
        // scale radius by making a point offset from mCenter, multiplying by mTransform, and taking the difference from center.
		GHPoint3 radPoint(mLocalSphere.mCenter);
		radPoint[0] += mLocalSphere.mRadius;
		mTransform->getTransform().mult(radPoint, radPoint);
		radPoint -= mWorldSphere.mCenter;
		mWorldSphere.mRadius = radPoint.length();
        mWorldVersion = mTransform->getTransform().getVersion();
    }
    return mWorldSphere;
}

void GHSphereBounds::setSphere(const GHSphere& sphere)
{
    mLocalSphere = sphere;
    mWorldVersion = -1;
}

void GHSphereBounds::setTransform(GHTransformNode* node)
{
    mTransform = node;
    mWorldVersion = -1;
}
