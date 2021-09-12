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
class GHTransformNode;

class GHRagdollToolSavedRagdollXMLLoader : public GHXMLObjLoader
{
public:
    GHRagdollToolSavedRagdollXMLLoader(const GHXMLObjFactory& objFatory,
                                       const GHIdentifierMap<int>& enumStore,
                                       const GHStringHashTable& hashtable,
                                       GHControllerMgr& controllerMgr,
                                       GHPhysicsSim& physicsSim,
                                       const GHXMLSerializer& xmlSerializer);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void loadRagdoll(GHRagdollGenerator& ret, GHTransformNode* parentSkeleton, GHXMLNode* node) const;
    void loadPhysicsObject(GHRagdollGenerator& ret, GHTransformNode* parentSkeleton, GHXMLNode* node, int index) const;
    void loadConstraint(GHRagdollGenerator& ret, GHXMLNode* node) const;
    
private:
    const GHXMLObjFactory& mObjFactory;
    const GHIdentifierMap<int>& mEnumStore;
    const GHStringHashTable& mHashtable;
    GHControllerMgr& mControllerMgr;
    GHPhysicsSim& mPhysicsSim;
    const GHXMLSerializer& mXMLSerializer;
};
