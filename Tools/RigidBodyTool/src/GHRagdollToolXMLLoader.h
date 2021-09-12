// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHIdentifierMap.h"

class GHXMLObjFactory;
class GHStringHashTable;
class GHControllerMgr;
class GHPhysicsSim;
class GHXMLSerializer;
class GHInputState;
class GHRagdollGenerator;

class GHRagdollToolXMLLoader : public GHXMLObjLoader
{
public:
    GHRagdollToolXMLLoader(const GHXMLObjFactory& objFatory,
                           const GHIdentifierMap<int>& enumStore,
                           const GHStringHashTable& hashtable,
                           GHControllerMgr& controllerMgr,
                           GHPhysicsSim& physicsSim,
                           const GHXMLSerializer& xmlSerializer);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void loadDescription(GHRagdollGenerator& ragdollGenerator, const GHXMLNode& node) const;
    
private:
    const GHXMLObjFactory& mObjFactory;
    const GHIdentifierMap<int>& mEnumStore;
    const GHStringHashTable& mHashtable;
    GHControllerMgr& mControllerMgr;
    GHPhysicsSim& mPhysicsSim;
    const GHXMLSerializer& mXMLSerializer;
};
