// Copyright Golden Hammer Software
#include "GHBulletOffsetModifierTool.h"
#include "GHInputState.h"
#include "GHBulletPhysicsObject.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHTransformNode.h"
#include "GHMath/GHMathDebugPrint.h"

GHBulletOffsetModifierTool::GHBulletOffsetModifierTool(const GHInputState& inputState,
                                                       GHBulletPhysicsObject& physicsObject)
: mInputState(inputState)
, mPhysicsObject(physicsObject)
{
    
}

void GHBulletOffsetModifierTool::onActivate(void)
{
    mPhysicsObject.getOffset().getTranslate(mCurrentOffset);
    mPhysicsObject.getOffset().calcYawPitchRoll(mCurrentRotationOffset[0], mCurrentRotationOffset[1], mCurrentRotationOffset[2]);

}

void GHBulletOffsetModifierTool::onDeactivate(void)
{
    
}

void GHBulletOffsetModifierTool::update(void)
{
    mThisFrameOffset.becomeIdentity();
    static float amount = .01f;
    float sign = 1.0f;
    if (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT))
    {
        sign = -1.f;
    }
    
    if (mInputState.checkKeyChange(0, 'y', true))
    {
        if (sign > 0) {
            amount *= 10.f;
        }
        else {
            amount /= 10.f;
        }
        GHDebugMessage::outputString("multiplier: %f", amount);
    }
    
    if (mInputState.checkKeyChange(0, '\'', true)
        || mInputState.checkKeyChange(0, '\"', true))
    {
        std::vector<GHTransformNode*> nodes = mPhysicsObject.getTransformNode()->getChildren();
        GHTransformNode* child = nodes[0];
        GHPoint3 axis;
        child->getLocalTransform().getTranslate(axis);
        axis.normalize();
        mThisFrameOffset.becomeAxisAngleRotation(axis, sign*amount);
    }
    else if (mInputState.checkKeyChange(0, ',', true)
        || mInputState.checkKeyChange(0, '<', true))
    {
        //mCurrentRotationOffset[0] += sign*amount;
        mThisFrameOffset.becomeXRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, '.', true)
             || mInputState.checkKeyChange(0, '>', true))
    {
        //mCurrentRotationOffset[1] += sign*amount;
        mThisFrameOffset.becomeYRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, 'p', true))
    {
        //mCurrentRotationOffset[2] += sign*amount;
        mThisFrameOffset.becomeZRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, 'o', true))
    {
        //mCurrentOffset[0] += sign*amount;
        mThisFrameOffset.setTranslate(GHPoint3(sign*amount, 0, 0));
    }
    else if(mInputState.checkKeyChange(0, 'e', true))
    {
        //mCurrentOffset[1] += sign*amount;
        mThisFrameOffset.setTranslate(GHPoint3(0, sign*amount, 0));
    }
    else if (mInputState.checkKeyChange(0, 'u', true))
    {
        //mCurrentOffset[2] += sign*amount;
        mThisFrameOffset.setTranslate(GHPoint3(0, 0, sign*amount));
    }
    else
    {
        return;
    }
    
    pushChange();
}

void GHBulletOffsetModifierTool::pushChange(void)
{
    /*
    GHTransform offset;
    offset.becomeYawPitchRollRotation(mCurrentRotationOffset[0], mCurrentRotationOffset[1], mCurrentRotationOffset[2]);
    offset.setTranslate(mCurrentOffset);
    mPhysicsObject.setOffset(offset);
     */
    
    mThisFrameOffset.mult(mPhysicsObject.getOffset(), mThisFrameOffset);
    mPhysicsObject.setOffset(mThisFrameOffset);
    
    mThisFrameOffset.calcYawPitchRoll(mCurrentRotationOffset[0], mCurrentRotationOffset[1], mCurrentRotationOffset[2]);
    mThisFrameOffset.getTranslate(mCurrentOffset);
    
    GHDebugMessage::outputString("(%f %f %f), (%f %f %f)", mCurrentOffset[0], mCurrentOffset[1], mCurrentOffset[2], mCurrentRotationOffset[0], mCurrentRotationOffset[1], mCurrentRotationOffset[2]);
    GHMathDebugPrint::printPoint(mThisFrameOffset.getMatrix(), "offsetTransform:");
}

GHBulletOffsetModifierTool* GHBulletOffsetModifierTool::clone(std::vector<GHBulletPhysicsObject*>& objectList) const
{
    size_t numObjects = objectList.size();
    for (size_t i = 0; i < numObjects; ++i)
    {
        GHBulletPhysicsObject* object = objectList[i];
        if (object->getTransformNode()->getId() == mPhysicsObject.getTransformNode()->getId())
        {
            return new GHBulletOffsetModifierTool(mInputState, *object);
        }
    }
    return 0;
}
