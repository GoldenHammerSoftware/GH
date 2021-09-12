// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTransform.h"
#include <vector>
#include "GHIdentifier.h"

class GHEntity;
class GHXMLObjFactory;

class CGBase
{
public:
    CGBase(const GHXMLObjFactory& objFactory,
           GHEntity& baseEnt,
           GHIdentifier activeStateId);
    virtual ~CGBase(void);
    
    void addUpgrade(const char* filename);
    
private:
    const GHXMLObjFactory& mObjFactory;
    GHEntity& mBaseEnt;
    GHIdentifier mActiveStateId;
    
    std::vector<GHEntity*> mUpgrades;
    std::vector<GHTransform> mUpgradeSlots;
};
