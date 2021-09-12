// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <map>
#include <vector>

class GHBulletRagdoll;
class GHTransformNode;
class GHBulletPhysicsObject;
class GHBulletRagdollConstraint;
class GHXMLSerializer;
class GHStringIdFactory;
class GHBulletObjectXMLLoader;
class GHBulletPhysicsSim;
class GHIdentifier;
class GHControllerMgr;
class GHInputState;

class GHBulletRagdollXMLLoader : public GHXMLObjLoader
{
public:
    GHBulletRagdollXMLLoader(GHBulletObjectXMLLoader& bulletObjectLoader,
                             GHBulletPhysicsSim& physicsSim,
                             const GHXMLSerializer& xmlSerializer,
                             const GHStringIdFactory& hashTable,
                             GHControllerMgr& controllerMgr,
                             const GHInputState& inputState);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
private:
    void createBone(GHBulletRagdoll& ret, GHTransformNode* skeleton, const GHXMLNode& xmlNode, GHPropertyContainer& extraData) const;
    GHTransformNode* findTransformNodeByID(GHTransformNode* skeleton, const GHIdentifier& boneID) const;
    GHBulletPhysicsObject* findRigidBodyByID(const GHIdentifier& boneID, int& outIndex) const;
    
    void createConstraint(GHBulletRagdoll& ret, const GHXMLNode& xmlNode) const;
    
private:
    GHBulletObjectXMLLoader& mBulletObjectLoader;
    GHBulletPhysicsSim& mPhysicsSim;
    const GHXMLSerializer& mXMLSerializer;
    const GHStringIdFactory& mIdFactory;
    GHControllerMgr& mControllerMgr;
    const GHInputState& mInputState;
    
    
    //For storing the bones and constraint info
    // during the load for easy access so that
    // we can easily fix up the constraints.
    //This data should be emptied after every load.
    typedef std::vector<GHBulletPhysicsObject*> BoneList;
    mutable BoneList mBoneList;
};
