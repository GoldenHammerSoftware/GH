#ifndef GH_DX12

#include "GHOculusDX11HMDServices.h"
#include "GHRenderServicesDX11.h"
#include "GHRenderDevice.h"
#include "GHSystemServices.h"
#include "GHOculusDX11HMDRenderDevice.h"
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

GHOculusDX11HMDServices::GHOculusDX11HMDServices(GHRenderServicesDX11& renderServices, 
	GHSystemServices& systemServices, GHControllerMgr& controllerMgr)
	: mRenderServices(renderServices)
	, mControllerMgr(controllerMgr)
	, mSystemServices(systemServices)
	, mOculusSystem(mSystemServices.getEventMgr())
{
	mControllerMgr.add(&mOculusSystem);
	// device must be initialized before hmddevice.
	assert(renderServices.getDevice());
	mHMDRenderDevice = new GHOculusDX11HMDRenderDevice(renderServices, mOculusSystem, systemServices.getEventMgr());
	mRenderServices.getDevice()->setHMDDevice(mHMDRenderDevice);

	mAvatarPoserFactory = new GHOculusAvatarPoserFactory(mOculusSystem);

	// renderdevice isStereo now means we are currently rendering a stereo pass
	//ghRenderDevice->setIsStereo(true);
}

GHOculusDX11HMDServices::~GHOculusDX11HMDServices(void)
{
	mControllerMgr.remove(&mOculusSystem);
	if (mTouchInputTranslator) { mControllerMgr.remove(mTouchInputTranslator); }
	delete mHMDRenderDevice;
	delete mTouchInputTranslator;
	delete mAvatarPoserFactory;
}

void GHOculusDX11HMDServices::initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal)
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

GHController* GHOculusDX11HMDServices::createHMDTransformApplier(GHTransform& outTrans)
{
	GHOculusHMDTransformApplier* ret = new GHOculusHMDTransformApplier(*mHMDRenderDevice, *mHMDOrigin, outTrans);
	return ret;
}

#endif
