// Copyright Golden Hammer Software
#include "GHPhysicsObject.h"
#include "GHTransformNode.h"
#include "GHMath/GHFloat.h"

void GHPhysicsObject::setTransformNode(GHTransformNode* transformNode)
{
    mTransformNode = transformNode;
	if (!mTransformNode) {
		mLastTransformVersion = -1;
		return;
	}
    mLastTransformVersion = mTransformNode->getTransform().getVersion();
}

bool GHPhysicsObject::isIdentifiedBy(const GHIdentifier& identifier) const
{
    return mTransformNode->getId() == identifier;
}

const GHTransform& GHPhysicsObject::getTransform(void) const
{
    return mTransformNode->getTransform();
}

void GHPhysicsObject::setTransform(const GHTransform& transform)
{
    if (!mTransformNode) {
        return;
    }
    
    if (mTransformNode->getParent())
    {
        GHTransformNode* parentNode = mTransformNode->getParent();
        GHTransform parentTransInv = parentNode->getTransform();
        parentTransInv.invert();
        assert(!GHFloat::isNaN(parentTransInv.getMatrix()));
        GHTransform localTransform;
        transform.mult(parentTransInv, localTransform);
        assert(!GHFloat::isNaN(localTransform.getMatrix()));
        mTransformNode->getLocalTransform() = localTransform;
    }
    else
    {
        mTransformNode->getLocalTransform() = transform;
    }
    
    mLastTransformVersion = mTransformNode->getTransform().getVersion();
}

void GHPhysicsObject::applyForce(const GHPoint3& force, const GHPoint3& position)
{
    mForces.push_back(Force(force, position));
}

void GHPhysicsObject::applyImpulse(const GHPoint3& impulse, const GHPoint3& position)
{
    mImpulses.push_back(Force(impulse, position));
}

void GHPhysicsObject::applyTorque(const GHPoint3& torque)
{
    mTorque += torque;
}

void GHPhysicsObject::applyAngularImpulse(const GHPoint3& impulse)
{
    mAngularImpulse += impulse;
}

void GHPhysicsObject::resetMotion(void)
{
    resetFrameData();
    mWantMotionReset = true;
}

const GHTransform* GHPhysicsObject::getManuallyPositionedTransform(void)
{
    if (mTransformNode->getTransform().getVersion() != mLastTransformVersion)
    {
        return &mTransformNode->getTransform();
    }
    else return 0;
}

void GHPhysicsObject::resetFrameData(void)
{
    mForces.resize(0);
    mImpulses.resize(0);
    mWantMotionReset = false;
    mTorque.setCoords(0,0,0);
    mAngularImpulse.setCoords(0,0,0);
}

void GHPhysicsObject::negateManualPosition(void)
{
    mLastTransformVersion = mTransformNode->getTransform().getVersion();
}
