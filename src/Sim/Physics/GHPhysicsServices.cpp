// Copyright Golden Hammer Software
#include "GHPhysicsServices.h"
#include "GHAppShard.h"
#include "GHMultiPhysicsObjectXMLLoader.h"

void GHPhysicsServices::initAppShard(GHAppShard& appShard, GHPhysicsSim& physicsSim, const GHIdentifierMap<int>& enumStore)
{
	GHMultiPhysicsObjectXMLLoader* multiPhysicsLoader = new GHMultiPhysicsObjectXMLLoader(appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(multiPhysicsLoader, 1, "physicsList");
}
