// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHIdentifier.h"
#include "GHRagdollGeneratorDescription.h"
#include <map>
#include "GHIdentifierMap.h"

class GHModel;
class GHBulletRagdoll;
class GHPhysicsSim;
class GHControllerMgr;
class GHIdentifier;
class GHTransformNode;
class GHBulletPhysicsObject;
class GHEntity;
class btCollisionShape;
class GHTransform;
class GHRagdollToolNode;
class GHXMLObjFactory;
class GHXMLNode;
class GHTransition;

//Generates a ragdoll skeleton.
// Todo: make not be GHBullet-specific
class GHRagdollGenerator
{
public:
    GHRagdollGenerator(const GHXMLObjFactory& objFactory,
                       const GHIdentifierMap<int>& enumStore,
                       GHEntity* targetEnt,
                       GHPhysicsSim& physicsSim,
                       GHControllerMgr& controllerMgr,
                       const GHIdentifier& activeID,
                       GHXMLNode* source=0);
    
    ~GHRagdollGenerator(void);
    
    GHRagdollToolNode* getNode(const GHIdentifier& id);
    
    //Set from the loader that loads the sparse description
    // from which the generation happens
    GHRagdollGeneratorDescription mDescription;
    
    //Called from the loader that loads the sparse description
    // from which generation happens
    void generateRagdoll(void);

    //Called from the loader that loads the verbose final
    // ragdoll map
    void setNode(const GHIdentifier& id, GHRagdollToolNode* node);
    void buildPregeneratedRagdoll(void);
    
    //Called from the tool nodes when stuff changes.
    void replaceRigidBody(int index, GHBulletPhysicsObject* newObj);
    
    const GHXMLNode* getXMLNode(void) const { return mTopXMLNode; }
    
    void reset(GHEntity* targetEnt);
    
    void updatePivotTransforms(const GHIdentifier& node, const GHTransform& oldTransform, const GHTransform& newTransformInv);
    
    GHEntity* getTargetEnt(void) { return mTargetEnt; }
    
private:
    void setEntity(GHEntity* targetEnt);
    void attachRagdoll(void);
    void detachRagdoll(void);
    void replaceSkeleton(GHTransformNode* skeleton);
    
private:
    GHXMLNode* mTopXMLNode;
    const GHXMLObjFactory& mXMLObjFactory;
    const GHIdentifierMap<int>& mEnumStore;
    GHEntity* mTargetEnt;
    GHModel* mTargetModel;
    GHTransition* mModelPhysicsTransition;
    GHIdentifier mActiveID;
    GHPhysicsSim& mPhysicsSim;
    GHControllerMgr& mControllerMgr;
    GHBulletRagdoll* mRagdoll;
    
private:
    typedef std::map<GHIdentifier, GHRagdollToolNode*> NodeMap;
    NodeMap mPhysicsObjects;
};
