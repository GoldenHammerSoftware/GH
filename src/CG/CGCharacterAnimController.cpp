// Copyright 2010 Golden Hammer Software
#include "CGCharacterAnimController.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "CGProperties.h"

CGCharacterAnimController::CGCharacterAnimController(const GHEntity& entity,
                                                     GHModel& model,
                                                     const GHIdentifier& idleAnim,
                                                     const GHIdentifier& runAnim,
                                                     const GHIdentifier& selectedAnim,
                                                     const GHIdentifier& attackAnim,
                                                     const GHIdentifier& deathAnim)
: mEntity(entity)
, mModel(model)
, mIdleAnim(idleAnim)
, mRunAnim(runAnim)
, mSelectedAnim(selectedAnim)
, mAttackAnim(attackAnim)
, mDeathAnim(deathAnim)
{
    
}

void CGCharacterAnimController::update(void)
{
    if ((int)mEntity.mProperties.getProperty(CGProperties::PP_HITPOINTS) == 0)
    {
        mModel.setAnim(mDeathAnim);
    }
    else if (mEntity.mProperties.getProperty(CGProperties::PP_ISATTACKING))
    {
        mModel.setAnim(mAttackAnim);
    }
    else if (mEntity.mProperties.getProperty(CGProperties::PP_HASDESTINATION))
    {
        mModel.setAnim(mRunAnim);
    }
    else if (mEntity.mProperties.getProperty(CGProperties::PP_ISSELECTED))
    {
        mModel.setAnim(mSelectedAnim);
    }
    else 
    {
        mModel.setAnim(mIdleAnim);
    }
}

void CGCharacterAnimController::onActivate(void)
{
    mModel.setAnim(mIdleAnim);
}

void CGCharacterAnimController::onDeactivate(void)
{
    
}