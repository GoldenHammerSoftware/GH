#include "GHOculusHMDServices.h"

#include "GHRenderServices.h"
#include "GHRenderDevice.h"
#include "GHSystemServices.h"
#include "GHOculusHMDRenderDevice.h"
#include "GHOculusHMDTransformApplier.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHOculusTouchInputTranslator.h"
#include "GHOculusAvatar.h"
#include "GHAppShard.h"
#include "GHOculusAvatarXMLLoader.h"
#include "GHHMDRecenterControllerXMLLoader.h"
#include "OVR_Platform.h"
#include "GHOculusAvatarPoser.h"
#include "GHRenderProperties.h"

GHOculusHMDServices::GHOculusHMDServices(GHRenderServices& renderServices,
	GHSystemServices& systemServices, GHControllerMgr& controllerMgr)
	: mRenderServices(renderServices)
	, mControllerMgr(controllerMgr)
	, mSystemServices(systemServices)
	, mOculusSystem(mSystemServices.getEventMgr())
{
	mControllerMgr.add(&mOculusSystem);
	mAvatarPoserFactory = new GHOculusAvatarPoserFactory(mOculusSystem);

	// subclasses should initialize mHMDRenderDevice and call mRenderServices.getDevice()->setHMDDevice().
}

GHOculusHMDServices::~GHOculusHMDServices(void)
{
	mControllerMgr.remove(&mOculusSystem);
	if (mTouchInputTranslator) { mControllerMgr.remove(mTouchInputTranslator); }
	delete mHMDRenderDevice;
	delete mTouchInputTranslator;
	delete mAvatarPoserFactory;
}

void GHOculusHMDServices::initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal)
{
	mHMDOrigin = &hmdOrigin;
	// todo? move some things out of hmdrenderdevice into here.

	// todo: get a real app id
#define MIRROR_SAMPLE_APP_ID "958062084316416"
	const char* appID = MIRROR_SAMPLE_APP_ID;

	//platform sdk stuff. 
	ovrPlatformInitializeResult result = ovr_PlatformInitializeWindows(appID);
	if (result != ovrPlatformInitialize_Success)
	{
		//error
	}

	ovrID userID = ovr_GetLoggedInUserID();

	appShard.mXMLObjFactory.addLoader(new GHOculusAvatarXMLLoader(appID, userID, appShard.mXMLObjFactory, *mHMDOrigin, *mAvatarPoserFactory,
		*mRenderServices.getVBFactory(), *mRenderServices.getMaterialCallbackMgr(), mSystemServices.getPlatformServices().getIdFactory(),
		mSystemServices.getInputState(), appShard.mProps, mSystemServices.getXMLSerializer(), mSystemServices.getEventMgr(), appShard.mEventMgr,
		appShard.mResourceFactory),
		1, "hmdAvatarController");

	appShard.mXMLObjFactory.addLoader(new GHHMDRecenterControllerXMLLoader(realTimeVal, const_cast<GHTransform&>(hmdOrigin), appShard.mProps, mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getInputState(), mSystemServices.getEventMgr(), 1),
		1, "vrRecenterController");

	mTouchInputTranslator = new GHOculusTouchInputTranslator(hmdOrigin, mOculusSystem, mSystemServices.getInputState());
	mControllerMgr.add(mTouchInputTranslator);
}

GHController* GHOculusHMDServices::createHMDTransformApplier(GHTransform& outTrans)
{
	GHOculusHMDTransformApplier* ret = new GHOculusHMDTransformApplier(*mHMDRenderDevice, *mHMDOrigin, outTrans);
	return ret;
}
