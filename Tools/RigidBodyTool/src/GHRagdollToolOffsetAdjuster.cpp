// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolOffsetAdjuster.h"
#include "GHInputState.h"
#include "GHRagdollToolNode.h"
#include "GHKeyDef.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"
#include "GHTransform.h"

GHRagdollToolOffsetAdjuster::GHRagdollToolOffsetAdjuster(const GHInputState& inputState,
                                                         GHRagdollToolNode*& currentSelection,
                                                         GHPropertyContainer& props)
: mInputState(inputState)
, mCurrentSelection(currentSelection)
, mProps(props)
{
    
}

void GHRagdollToolOffsetAdjuster::onActivate(void)
{
    
}

void GHRagdollToolOffsetAdjuster::onDeactivate(void)
{
    
}

void GHRagdollToolOffsetAdjuster::update(void)
{
    if (!mCurrentSelection) { return; }
    
    GHTransform adjustment;
    adjustment.becomeIdentity();
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
        snprintf(mMessageBuf, BUFLEN, "multiplier: %f", amount);
        mProps.setProperty(GHRagdollToolProperties::COMMANDRESULT, mMessageBuf);
    }
    
    if (mInputState.checkKeyChange(0, ',', true)
             || mInputState.checkKeyChange(0, '<', true))
    {
        adjustment.becomeXRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, '.', true)
             || mInputState.checkKeyChange(0, '>', true))
    {
        adjustment.becomeYRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, 'p', true))
    {
        adjustment.becomeZRotation(sign*amount);
    }
    else if (mInputState.checkKeyChange(0, 'o', true))
    {
        adjustment.setTranslate(GHPoint3(sign*amount, 0, 0));
    }
    else if(mInputState.checkKeyChange(0, 'e', true))
    {
        adjustment.setTranslate(GHPoint3(0, sign*amount, 0));
    }
    else if (mInputState.checkKeyChange(0, 'u', true))
    {
        adjustment.setTranslate(GHPoint3(0, 0, sign*amount));
    }
    else
    {
        return;
    }
    
    mCurrentSelection->applyOffsetChange(adjustment);
}
