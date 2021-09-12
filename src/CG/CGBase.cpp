// Copyright 2010 Golden Hammer Software
#include "CGBase.h"
#include "GHEntity.h"
#include "GHXMLObjFactory.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "CGProperties.h"
#include "GHDebugMessage.h"

CGBase::CGBase(const GHXMLObjFactory& objFactory,
               GHEntity& baseEnt,
               GHIdentifier activeStateId)
: mObjFactory(objFactory)
, mBaseEnt(baseEnt)
, mActiveStateId(activeStateId)
{
    mUpgradeSlots.reserve(5);
    
    float offset = .5f;
    float yOffset = .5f;
    
    GHTransform transform;
    transform.becomeIdentity();
    transform.setTranslate(GHPoint3(offset, yOffset, 0));
    mUpgradeSlots.push_back(transform);
    transform.setTranslate(GHPoint3(0, yOffset, offset));
    mUpgradeSlots.push_back(transform);
    transform.setTranslate(GHPoint3(-offset, yOffset, 0));
    mUpgradeSlots.push_back(transform);
    transform.setTranslate(GHPoint3(0, yOffset, -offset));
    mUpgradeSlots.push_back(transform);
    transform.setTranslate(GHPoint3(0, .75f, 0));
    mUpgradeSlots.push_back(transform);
}

CGBase::~CGBase(void)
{
    size_t numUpgrades = mUpgrades.size();
    for (size_t i = 0; i < numUpgrades; ++i)
    {
        GHEntity* upgrade = mUpgrades[i];
        delete upgrade;
    }
}

void CGBase::addUpgrade(const char* filename)
{
    if (mUpgrades.size() < mUpgradeSlots.size())
    {
        GHEntity* upgrade = (GHEntity*)mObjFactory.load(filename);
        if (!upgrade) { return; }
        GHModel* upgradeModel = upgrade->mProperties.getProperty(GHEntityHashes::MODEL);
        if (!upgradeModel) { return; }
        
        const GHTransform& offsetTransform = mUpgradeSlots[mUpgrades.size()];
        GHModel* parentModel = mBaseEnt.mProperties.getProperty(GHEntityHashes::MODEL);
        const GHTransform& parentTransform = parentModel->getSkeleton()->getTransform();
        
        upgrade->mProperties.setProperty(CGProperties::PP_TEAM, mBaseEnt.mProperties.getProperty(CGProperties::PP_TEAM));
        upgrade->mProperties.setProperty(CGProperties::PP_ENTCOLOR, mBaseEnt.mProperties.getProperty(CGProperties::PP_ENTCOLOR));
               
        GHPoint3 pos;
        parentTransform.getTranslate(pos);
        
        GHPoint3 offsetPos;
        offsetTransform.getTranslate(offsetPos);
        pos += offsetPos;
        
        upgradeModel->getSkeleton()->getLocalTransform().setTranslate(pos);
        
        upgrade->mStateMachine.setState(mActiveStateId);
        
        mUpgrades.push_back(upgrade);
    }
}
