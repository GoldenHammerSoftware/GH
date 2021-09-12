// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPoint.h"
#include "GHTransform.h"

class GHRagdollToolNode;
class GHInputState;
class btRigidBody;
class GHTransform;

class GHRagdollToolPivotDisplayer : public GHController
{
public:
    GHRagdollToolPivotDisplayer(const GHInputState& inputState, GHRagdollToolNode*& currentSelection);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    void toggleDisplay(void);
    void updateDisplay(void);
    
private:
    class PivotDisplay
    {
    public:
        PivotDisplay(const GHPoint3& color);
        void setIsShowing(bool showing);
        void updatePosition(const GHTransform& transform);
    private:
        GHPoint3 mColor;
        int mLineID1;
        int mLineID2;
        bool mIsShowing;
    };
    
    void updateDisplay(const char* attr, const btRigidBody& rigidBody, PivotDisplay& display);
    
private:
    const GHInputState& mInputState;
    GHRagdollToolNode*& mCurrentSelection;
    GHRagdollToolNode* mTarget;
    int mToggle;
    PivotDisplay mPivotInA;
    PivotDisplay mPivotInB;
    
    GHTransform lastPivotLocalFrame;
    GHTransform lastRigidTransform;
    GHTransform lastPivotWorldFrame;
};
