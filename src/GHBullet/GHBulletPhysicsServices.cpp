// Copyright Golden Hammer Software
#include "GHBulletPhysicsServices.h"
#include "GHAppShard.h"
#include "GHBulletPhysicsSim.h"
#include "GHBulletObjectXMLLoader.h"
#include "GHBulletRagdollXMLLoader.h"
#include "GHBulletVehicleXMLLoader.h"
#include "GHBulletTextureLookupFrictionOverrideXMLLoader.h"
#include "GHSystemServices.h"


GHBulletPhysicsServices::GHBulletPhysicsServices(GHSystemServices& systemServices, const GHPoint3& gravity)
: mGravity(gravity)
, mSystemServices(systemServices) 
{
}

GHPhysicsSim* GHBulletPhysicsServices::createPhysicsSim(const GHTimeVal& timeVal)
{
    return new GHBulletPhysicsSim(timeVal, mGravity);
}

void GHBulletPhysicsServices::initAppShard(GHAppShard& appShard, GHPhysicsSim& physicsSim, const GHIdentifierMap<int>& enumStore)
{
	GHPhysicsServices::initAppShard(appShard, physicsSim, enumStore);

    GHBulletObjectXMLLoader* bulletObjectLoader = new GHBulletObjectXMLLoader((GHBulletPhysicsSim&)physicsSim, enumStore, mSystemServices.getPlatformServices().getIdFactory(), appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(bulletObjectLoader, 1, "physics");
    
    GHBulletRagdollXMLLoader* ragdollLoader = new GHBulletRagdollXMLLoader(*bulletObjectLoader, (GHBulletPhysicsSim&)physicsSim, mSystemServices.getXMLSerializer(), mSystemServices.getPlatformServices().getIdFactory(), appShard.mControllerMgr, appShard.mSystemServices.getInputState());
    appShard.mXMLObjFactory.addLoader(ragdollLoader, 1, "ragdoll");
    
    GHBulletVehicleXMLLoader* vehicleLoader = new GHBulletVehicleXMLLoader(*bulletObjectLoader, (GHBulletPhysicsSim&)physicsSim, appShard.mControllerMgr, mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(vehicleLoader, 1, "vehicle");

	GHBulletTextureLookupFrictionOverrideXMLLoader* frictionOverrideLoader = new GHBulletTextureLookupFrictionOverrideXMLLoader(appShard.mResourceFactory);
	appShard.mXMLObjFactory.addLoader(frictionOverrideLoader, 1, "contactAddedTextureLookupFriction");
}
