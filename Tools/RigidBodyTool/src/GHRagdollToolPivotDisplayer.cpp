// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolPivotDisplayer.h"
#include "GHInputState.h"
#include "GHRagdollToolNode.h"
#include "GHDebugDraw.h"
#include "GHXMLNode.h"
#include "GHTransform.h"
#include "GHBulletRagdollConstraint.h"
#include "btBulletDynamicsCommon.h"
#include "GHBulletUtil.h"
#include "GHFloat.h"

GHRagdollToolPivotDisplayer::GHRagdollToolPivotDisplayer(const GHInputState& inputState, GHRagdollToolNode*& currentSelection)
: mInputState(inputState)
, mCurrentSelection(currentSelection)
, mTarget(0)
, mToggle(0)
, mPivotInA(GHPoint3(1, 0, 0))
, mPivotInB(GHPoint3(0, 1, 0))
{
    
}

void GHRagdollToolPivotDisplayer::onActivate(void)
{
    toggleDisplay();
}

void GHRagdollToolPivotDisplayer::onDeactivate(void)
{

}

void GHRagdollToolPivotDisplayer::update(void)
{
    if (mInputState.checkKeyChange(0, '2', true))
    {
        if (mCurrentSelection &&
            mTarget != mCurrentSelection)
        {
            mToggle = 0;
            mTarget = mCurrentSelection;
        }
        
        mToggle = (mToggle + 1) % 4;
        toggleDisplay();
    }
    
    updateDisplay();
}

void GHRagdollToolPivotDisplayer::updateDisplay(void)
{
    if (!mTarget
        || !mTarget->getConstraint()) { return; }
    
    const btRigidBody& rbA = mTarget->getConstraint()->getBulletConstraint()->getRigidBodyA();
    const btRigidBody& rbB = mTarget->getConstraint()->getBulletConstraint()->getRigidBodyB();
    
    updateDisplay("pivotInATrans", rbA, mPivotInA);
    updateDisplay("pivotInBTrans", rbB, mPivotInB);
}

void GHRagdollToolPivotDisplayer::updateDisplay(const char* attr, const btRigidBody& rigidBody, PivotDisplay& display)
{
    GHTransform pivotLocalFrame;
    mTarget->getConstraintNode()->readAttrFloatArr(attr, pivotLocalFrame.getMatrix().getArr(), 16);

    GHTransform ghRigidTrans;
    const btTransform& rigidTrans = rigidBody.getWorldTransform();
    GHBulletUtil::bulletToGHTransform(rigidTrans, ghRigidTrans);
    
    GHTransform pivotWorldFrame;
    pivotLocalFrame.mult(ghRigidTrans, pivotWorldFrame);
    
    display.updatePosition(pivotWorldFrame);
}

void GHRagdollToolPivotDisplayer::toggleDisplay(void)
{
    //0 = show nothing
    //1 = show A
    //2 = show B
    //3 = show both
    
    bool showPivotInA = mToggle == 1 || mToggle == 3;
    bool showPivotInB = mToggle == 2 || mToggle == 3;
    
    mPivotInA.setIsShowing(showPivotInA);
    mPivotInB.setIsShowing(showPivotInB);
}

GHRagdollToolPivotDisplayer::GHRagdollToolPivotDisplayer::PivotDisplay::PivotDisplay(const GHPoint3& color)
: mColor(color)
, mLineID1(-1)
, mLineID2(-1)
, mIsShowing(false)
{

}

void GHRagdollToolPivotDisplayer::PivotDisplay::setIsShowing(bool showing)
{
    if ((showing && mIsShowing)
        || (!showing && !mIsShowing))
    { return; }
    
    if (showing)
    {
        GHPoint3 lhsPoint(0,0,0), rhsPoint(0,0,0);
        mLineID1 = GHDebugDraw::getSingleton().addLine(lhsPoint, rhsPoint, mColor);
        mLineID2 = GHDebugDraw::getSingleton().addLine(lhsPoint, rhsPoint, mColor);
    }
    else
    {
        GHDebugDraw::getSingleton().removeShape(mLineID2);
        GHDebugDraw::getSingleton().removeShape(mLineID1);
        mLineID1 = -1;
        mLineID2 = -1;
    }
    
    mIsShowing = showing;
}

void GHRagdollToolPivotDisplayer::PivotDisplay::updatePosition(const GHTransform& transform)
{
    if (!mIsShowing) { return; }
    
    GHPoint3 position;
    transform.getTranslate(position);
    
    
    //To display a line pointing down the pivot's local space z-axis.
    GHPoint3 direction(1,0,0);
    transform.multDir(direction, direction);
    direction += position;
    GHDebugDraw::getSingleton().setLine(mLineID1, position, direction, mColor);
    GHDebugDraw::getSingleton().setLine(mLineID2, position, direction, mColor);
    
    //To display a crosshair where the pivot is
    /*
    GHPoint3 lhsPoint(position[0], position[1]+.01, position[2]);
    GHPoint3 rhsPoint(position[0], position[1]-.01, position[2]);
    GHDebugDraw::getSingleton().setLine(mLineID1, lhsPoint, rhsPoint, mColor);
    
    lhsPoint.setCoords(position[0], position[1], position[2]+.01);
    rhsPoint.setCoords(position[0], position[1], position[2]-.01);
    GHDebugDraw::getSingleton().setLine(mLineID2, lhsPoint, rhsPoint, mColor);
     */
}
