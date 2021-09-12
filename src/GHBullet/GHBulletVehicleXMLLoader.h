// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

class GHBulletObjectXMLLoader;
class GHBulletPhysicsSim;
class GHControllerMgr;
class GHStringIdFactory;

class GHBulletVehicleXMLLoader : public GHXMLObjLoader
{
public:
    GHBulletVehicleXMLLoader(GHBulletObjectXMLLoader& bulletObjectLoader,
                             GHBulletPhysicsSim& physicsSim,
                             GHControllerMgr& controllerMgr,
                             const GHStringIdFactory& hashTable,
                             const GHIdentifierMap<int>& enumMap);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHBulletObjectXMLLoader& mBulletObjectLoader;
    GHBulletPhysicsSim& mPhysicsSim;
    GHControllerMgr& mControllerMgr;
    const GHStringIdFactory& mIdFactory;
    const GHIdentifierMap<int>& mEnumMap;
};
