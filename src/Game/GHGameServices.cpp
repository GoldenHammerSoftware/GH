// Copyright Golden Hammer Software
#include "GHGameServices.h"
#include "GHCacheModelLoader.h"
#include "GHModelLoader.h"
#include "GHMD3ModelLoader.h"
#include "GHAMModelLoader.h"
#include "GHFrameAnimSetLoader.h"
#include "GHSystemServices.h"
#include "GHRenderServices.h"
#include "GHEntityXMLLoader.h"
#include "GHModelPropertyXMLLoader.h"
#include "GHEntityModelRenderTransitionXMLLoader.h"
#include "GHEntityModelPhysicsTransitionXMLLoader.h"
#include "GHEntityModelAnimTransitionXMLLoader.h"
#include "GHEntityParticleTransitionXMLLoader.h"
#include "GHEntitySpawnTransitionXMLLoader.h"
#include "GHAppShard.h"
#include "GHResourceLoaderXML.h"
#include "GHHouseAdXMLLoader.h"
#include "GHHeightFieldXMLLoader.h"
#include "GHMaterialCallbackFactoryProperty.h"
#include "GHMaterialCallbackMgr.h"
#include "GHHeightFieldEntXMLLoader.h"
#include "GHTransformNodeXMLLoader.h"
#include "GHGameIdentifiers.h"
#include "GHAMAnimFactoryGPU.h"
#include "GHSkeletonDebugDrawerXMLLoader.h"
#include "GHNewsMgr.h"
#include "GHNewsGUITransitionXMLLoader.h"
#include "GHHeightFieldCollideModifierXMLLoader.h"
#include "GHHeightFieldHFFSaveModifierXMLLoader.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHRenderDevice.h"
#include "GHDelayControllerXMLLoader.h"

#define ENUMHEADER "GHEntityHashes.h" 
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHHeightFieldProperties.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHPhysicsShapeType.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHGameIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHStatTrackerIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHMultiplayerIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHCSIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHHFUVType.h"
#include "GHString/GHEnumInclude.h"

GHGameServices::GHGameServices(GHSystemServices& systemServices,
                               GHRenderServices& renderServices,
                               GHMessageHandler& appMessageQueue)
: mSystemServices(systemServices)
, mRenderServices(renderServices)
, mAppMessageQueue(appMessageQueue)
{
    GHEntityHashes::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
    GHHeightFieldProperties::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
    GHPhysicsShapeType::generateEnum(systemServices.getPlatformServices().getEnumStore());
    GHGameIdentifiers::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
    GHStatTrackerIdentifiers::generateIdentifiers(mSystemServices.getPlatformServices().getIdFactory());
    GHMultiplayerIdentifiers::generateIdentifiers(mSystemServices.getPlatformServices().getIdFactory());
    GHCSIdentifiers::generateIdentifiers(mSystemServices.getPlatformServices().getIdFactory());
	GHHFUVType::generateEnum(systemServices.getPlatformServices().getEnumStore());

    GHMaterialCallbackFactory* hfSizeCallback = new GHMaterialCallbackFactoryProperty("TextureWorldSize",GHMaterialCallbackType::CT_PERENT, GHHeightFieldProperties::HFTEXTUREWORLDSIZE, GHMaterialParamHandle::HT_FLOAT);
    mRenderServices.getMaterialCallbackMgr()->addFactory(hfSizeCallback);
}

void GHGameServices::initAppShard(GHAppShard& appShard,
                                  const GHTimeVal& realTime,
                                  const GHTimeVal& simTime,
                                  GHRenderGroup& renderGroup,
                                  GHPhysicsSim& physicsSim)
{
    GHCacheModelLoader* cacheModelLoader = new GHCacheModelLoader(appShard.mResourceFactory);
    appShard.mXMLObjFactory.addLoader(cacheModelLoader, 1, "cacheModel");
    
    GHModelLoader* modelLoader = new GHModelLoader(appShard.mXMLObjFactory, mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(modelLoader, 1, "model");
     
    GHMD3ModelLoader* md3Loader = new GHMD3ModelLoader(appShard.mXMLObjFactory,
                                                       mSystemServices.getPlatformServices().getIdFactory(),
                                                       mSystemServices.getPlatformServices().getFileOpener(),
                                                       *mRenderServices.getVBFactory(), 
                                                       simTime);
    appShard.mXMLObjFactory.addLoader(md3Loader, 1, "md3model");
    
    GHAMAnimFactory* ghamAnimFactory = new GHAMAnimFactoryGPU(simTime, mSystemServices.getPlatformServices().getIdFactory());
    GHAMModelLoader* ghamLoader = new GHAMModelLoader(appShard.mXMLObjFactory, mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getPlatformServices().getFileOpener(), *mRenderServices.getVBFactory(), mSystemServices.getPlatformServices().getEnumStore(), ghamAnimFactory);
    appShard.mXMLObjFactory.addLoader(ghamLoader, 1, "gham");
    
    GHFrameAnimSetLoader* animSetLoader = new GHFrameAnimSetLoader(mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(animSetLoader, 1, "animSet");
    
    GHEntityXMLLoader* entityLoader = new GHEntityXMLLoader(appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(entityLoader, 1, "entity");
    
    GHModelPropertyXMLLoader* modelPropLoader = new GHModelPropertyXMLLoader(appShard.mResourceFactory);
    appShard.mXMLObjFactory.addLoader(modelPropLoader, 1, "modelProp");
    
    GHEntityModelRenderTransitionXMLLoader* emrt = new GHEntityModelRenderTransitionXMLLoader(renderGroup, mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(emrt, 1, "entityRenderTransition");
    
    GHEntityModelPhysicsTransitionXMLLoader* empt = new GHEntityModelPhysicsTransitionXMLLoader(physicsSim, appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(empt, 1, "entityPhysicsTransition");
    
    GHEntityModelAnimTransitionXMLLoader* emat = new GHEntityModelAnimTransitionXMLLoader(mSystemServices.getPlatformServices().getIdFactory(), simTime);
    appShard.mXMLObjFactory.addLoader(emat, 1, "entityAnimTransition");
    
	GHEntityParticleTransitionXMLLoader* epart = new GHEntityParticleTransitionXMLLoader(appShard.mXMLObjFactory, 
		realTime, mRenderServices.getDevice()->getViewInfo(), *mRenderServices.getVBFactory(), mSystemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(epart, 1, "entityParticleTransition");

    GHEntitySpawnTransitionXMLLoader* spawnTrans = new GHEntitySpawnTransitionXMLLoader(mSystemServices.getPlatformServices().getIdFactory(), appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(spawnTrans, 1, "entitySpawn");
    
    GHSkeletonDebugDrawerXMLLoader* skeletonDebugDrawerLoader = new GHSkeletonDebugDrawerXMLLoader(appShard.mControllerMgr, mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(skeletonDebugDrawerLoader, 1, "skeletonDebugDrawer");
    
    const GHGUIButtonXMLLoader* buttonLoader = (GHGUIButtonXMLLoader*)appShard.mXMLObjFactory.getLoader("guiButton");
    if (buttonLoader)
    {
        GHIdentifier houseAdTransId = mSystemServices.getPlatformServices().getIdFactory().generateHash("GP_GUIHOUSEADTRANS");
        
        GHHouseAdXMLLoader* houseAdLoader = new GHHouseAdXMLLoader(*buttonLoader, mAppMessageQueue, houseAdTransId);
        appShard.mXMLObjFactory.addLoader(houseAdLoader, 1, "ghHouseAds");
    }
    
    GHHeightFieldXMLLoader* hfLoader = new GHHeightFieldXMLLoader(appShard.mResourceFactory);
    appShard.mXMLObjFactory.addLoader(hfLoader, 1, "heightField");
 
    GHHeightFieldEntXMLLoader* hfEntLoader = new GHHeightFieldEntXMLLoader(appShard.mXMLObjFactory, *hfLoader, *entityLoader, *mRenderServices.getVBFactory(), mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getPlatformServices().getThreadFactory(), mSystemServices.getPlatformServices().getEnumStore(), appShard.mResourceFactory);
    appShard.mXMLObjFactory.addLoader(hfEntLoader, 1, "heightFieldEnt");
    
	GHHeightFieldCollideModifierXMLLoader* hfColMod = new GHHeightFieldCollideModifierXMLLoader(physicsSim,
		appShard.mSystemServices.getPlatformServices().getEnumStore(), appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(hfColMod, 1, "hfCollideMod");

	GHHeightFieldHFFSaveModifierXMLLoader* hfSaveMod = new GHHeightFieldHFFSaveModifierXMLLoader(mSystemServices.getPlatformServices().getFileOpener());
	appShard.mXMLObjFactory.addLoader(hfSaveMod, 1, "hfSaveMod");
	
	GHTransformNodeXMLLoader* transLoader = new GHTransformNodeXMLLoader(appShard.mSystemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(transLoader, 1, "transformNode");

	GHNewsMgr* newsMgr = new GHNewsMgr(appShard.mSystemServices.getXMLSerializer(), appShard.mXMLObjFactory, appShard.mProps, appShard.mEventMgr);
	appShard.addOwnedItem(new GHTypedDeletionHandle<GHNewsMgr>(newsMgr));

	GHNewsGUITransitionXMLLoader* newsTrans = new GHNewsGUITransitionXMLLoader(*newsMgr);
	appShard.mXMLObjFactory.addLoader(newsTrans, 1, "ghnews");

	GHDelayControllerXMLLoader* delayLoader = new GHDelayControllerXMLLoader(realTime, appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(delayLoader, 1, "delayController");
}
