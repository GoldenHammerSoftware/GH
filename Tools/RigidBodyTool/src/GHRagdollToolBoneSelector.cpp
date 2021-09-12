// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolBoneSelector.h"
#include "GHInputState.h"
#include "GHRagdollToolNode.h"
#include "GHKeyDef.h"
#include "GHRagdollGenerator.h"
#include "GHPhysicsObject.h"
#include "GHTransformNode.h"
#include "GHDebugMessage.h"
#include "GHIdentifier.h"
#include "GHPropertyContainer.h"
#include "GHRagdollToolProperties.h"

GHRagdollToolBoneSelector::GHRagdollToolBoneSelector(const GHInputState& inputState,
                                                     const GHViewInfo& viewInfo,
                                                     GHPhysicsSim& physicsSim,
                                                     GHPropertyContainer& properties,
                                                     GHRagdollGenerator& ragdollGenerator,
                                                     GHRagdollToolNode*& currentSelection,
                                                     GHPhysicsObject*& currentPhysicsObject)
: mInputState(inputState)
, mPicker(inputState, viewInfo, physicsSim)
, mProperties(properties)
, mCurrentSelection(currentSelection)
, mCurrentPhysicsObject(currentPhysicsObject)
, mRagdollGenerator(ragdollGenerator)
{
    mProperties.setProperty(GHRagdollToolProperties::SELECTEDBONE, "Nothing");
}

void GHRagdollToolBoneSelector::onActivate(void)
{
    
}

void GHRagdollToolBoneSelector::onDeactivate(void)
{

}

void GHRagdollToolBoneSelector::update(void)
{
    if (mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE2, true))
    {
        GHPhysicsObject* obj = mPicker.pickObject();
        if (obj && obj->getTransformNode())
        {
            mCurrentSelection = mRagdollGenerator.getNode(obj->getTransformNode()->getId());
            mCurrentPhysicsObject = obj;
            
#ifdef DEBUG_IDENTIFIERS
            mProperties.setProperty(GHRagdollToolProperties::SELECTEDBONE, obj->getTransformNode()->getId().getString());
#else
            mProperties.setProperty(GHRagdollToolProperties::SELECTEDBONE, "Something");
#endif
        }
        else
        {
            mProperties.setProperty(GHRagdollToolProperties::SELECTEDBONE, "Nothing");
        }
    }
}
