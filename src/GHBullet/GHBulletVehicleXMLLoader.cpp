// Copyright Golden Hammer Software
#include "GHBulletVehicleXMLLoader.h"
#include "GHBulletObjectXMLLoader.h"
#include "GHBulletVehicle.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHBulletBtInclude.h"

GHBulletVehicleXMLLoader::GHBulletVehicleXMLLoader(GHBulletObjectXMLLoader& bulletObjectLoader,
                                                   GHBulletPhysicsSim& physicsSim,
                                                   GHControllerMgr& controllerMgr,
                                                   const GHStringIdFactory& hashTable,
                                                   const GHIdentifierMap<int>& enumMap)
: mBulletObjectLoader(bulletObjectLoader)
, mPhysicsSim(physicsSim)
, mControllerMgr(controllerMgr)
, mIdFactory(hashTable)
, mEnumMap(enumMap)
{
    
}

void* GHBulletVehicleXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const GHXMLNode* chassisNode = node.getChild("chassis", false);
    if (!chassisNode) {
        GHDebugMessage::outputString("failed to find chassis node in vehicle xml");
        return 0;
    }
    GHBulletPhysicsObject* chassis = (GHBulletPhysicsObject*)mBulletObjectLoader.create(*chassisNode, extraData);
    if (!chassis) {
        GHDebugMessage::outputString("failed to load chassis for vehicle");
        return 0;
    }
    
    GHIdentifier inContactProp;
    node.readAttrIdentifier("inContactProp", inContactProp, mIdFactory);
    
    GHIdentifier disableProp;
    node.readAttrIdentifier("disableProp", disableProp, mIdFactory);
    
    GHIdentifier wheelPointProp;
    node.readAttrIdentifier("wheelPointProp", wheelPointProp, mIdFactory);
    
    GHIdentifier wheelPointNormalProp;
    node.readAttrIdentifier("wheelPointNormalProp", wheelPointNormalProp, mIdFactory);
    
    float priority = 0;
    node.readAttrFloat("priority", priority);
    int priorityEnum = 0;
    if (node.readAttrEnum("priorityEnum", priorityEnum, mEnumMap)) {
        priority = (float)priorityEnum;
    }
    
    btRaycastVehicle::btVehicleTuning tuning;
    node.readAttrFloat("frictionSlip", tuning.m_frictionSlip);
    node.readAttrFloat("maxSuspensionForce", tuning.m_maxSuspensionForce);
    node.readAttrFloat("maxSuspensionTravelCm", tuning.m_maxSuspensionTravelCm);
    node.readAttrFloat("suspensionCompression", tuning.m_suspensionCompression);
    node.readAttrFloat("suspensionDamping", tuning.m_suspensionDamping);
    node.readAttrFloat("suspensionStiffness", tuning.m_suspensionStiffness);
   
    /* default vals
     
     frictionSlip, 10.500000
     maxSuspensionForce, 6000.000000
     maxSuspensionTravelCm, 500.000000
     suspensionCompression, 0.830000
     suspensionDamping, 0.880000
     suspensionStiffness, 5.880000
     */
    
    //tuning.m_maxSuspensionTravelCm = 10000;
    //tuning.m_suspensionCompression = .5;
    //tuning.m_suspensionStiffness = 10;

    
    //for making it not have any suspension
    //tuning.m_suspensionCompression = 0;
    //tuning.m_maxSuspensionTravelCm = 0;
    //tuning.m_frictionSlip = 0;
    
    
    tuning.m_suspensionDamping = 2.f*btSqrt(tuning.m_suspensionStiffness) * .5f;
    
    /*
    tuning.m_frictionSlip = 0;
    tuning.m_maxSuspensionForce = 0;
    tuning.m_maxSuspensionTravelCm = 0;
    tuning.m_suspensionCompression = 0;
    tuning.m_suspensionDamping = 0;
    tuning.m_suspensionStiffness = 0;
    */
    GHBulletVehicle* ret = new GHBulletVehicle(mPhysicsSim, mControllerMgr, chassis, tuning, inContactProp, disableProp, wheelPointProp, wheelPointNormalProp, priority);
    return ret;
}
