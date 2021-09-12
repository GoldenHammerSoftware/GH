#include "GHOpenVRDX11HMDServices.h"
#include "GHRenderServicesDX11.h"
#include "GHRenderDevice.h"
#include "GHSystemServices.h"
#include "GHOpenVRDX11HMDRenderDevice.h"
#include "GHOpenVRHMDTransformApplier.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHOpenVRInputHandler.h"
#include "GHAppShard.h"
#include "GHOculusAvatarXMLLoader.h"
//#include "GHHMDRecenterControllerXMLLoader.h"
#include <openvr.h>
#include "GHOpenVROculusAvatarPoser.h"
#include "GHInputStateOculusAvatarPoser.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHMessage.h"
#include "GHSystemServices.h"
#include "GHPlatform/GHFile.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHOpenVRSkeletalAvatarXMLLoader.h"

// For getting the absolute path of the input manifest.
// If we wanted to make this not depend on win32 we could create a GHFileOpener::getAbsolutePathRoot() function.
#include "GHPlatform/win32/GHWin32Include.h"

GHOpenVRDX11HMDServices::GHOpenVRDX11HMDServices(GHRenderServicesDX11& renderServices,
												 GHSystemServices& systemServices, GHControllerMgr& controllerMgr)
	: mRenderServices(renderServices)
	, mControllerMgr(controllerMgr)
	, mSystemServices(systemServices)
	, mOpenVR(systemServices.getEventMgr())
{
	mControllerMgr.add(&mOpenVR);
	// device must be initialized before hmddevice.
	assert(renderServices.getDevice());
	mHMDRenderDevice = new GHOpenVRDX11HMDRenderDevice(renderServices, mOpenVR, systemServices.getEventMgr());
	mRenderServices.getDevice()->setHMDDevice(mHMDRenderDevice);

	// Find the steamvr input manifest and let steam know about it.
	char actionsPath[GHFileOpener::MAX_PATH_LEN];
	GHFile* actionsFile = systemServices.getPlatformServices().getFileOpener().openFile("steamactions.json", GHFile::FT_TEXT, GHFile::FM_READONLY, actionsPath);
	if (actionsFile)
	{
		delete actionsFile;
		WCHAR absolutePathW[MAX_PATH];
		WCHAR localPathW[GHFileOpener::MAX_PATH_LEN];
		MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, actionsPath, GHFileOpener::MAX_PATH_LEN, localPathW, MAX_PATH);
		GetFullPathName(localPathW, MAX_PATH, absolutePathW, nullptr);
		char absolutePathC[MAX_PATH];
		WideCharToMultiByte(CP_UTF8, 0, absolutePathW, MAX_PATH, absolutePathC, MAX_PATH, NULL, NULL);
		vr::VRInput()->SetActionManifestPath(absolutePathC);
	}
}

GHOpenVRDX11HMDServices::~GHOpenVRDX11HMDServices(void)
{
	mControllerMgr.remove(&mOpenVR);
	if (mInputHandler) { mControllerMgr.remove(mInputHandler); }
	delete mHMDRenderDevice;
	delete mInputHandler;
	delete mAvatarPoserFactory;
}

void GHOpenVRDX11HMDServices::initAppShard(GHAppShard& appShard, const GHTransform& hmdOrigin, const GHTimeVal& realTimeVal)
{
	mHMDOrigin = &hmdOrigin;
	// todo? move some things out of hmdrenderdevice into here.

	// Using GHInputStateOculusAvatarPoserFactory for now.
	// If we come up with a reason to have a separate version for openvr then finish implementing GHOpenVROculusAvatarPoserFactory
	//mAvatarPoserFactory = new GHOpenVROculusAvatarPoserFactory(mSystemServices.getInputState());
	mAvatarPoserFactory = new GHInputStateOculusAvatarPoserFactory(hmdOrigin, mSystemServices.getInputState(), realTimeVal);

	//TOODO: Probably in an avatar class somewhere, call this:
	//m_pRenderModels = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	//if (!m_pRenderModels)
	//{
	//	m_pHMD = NULL;
	//	vr::VR_Shutdown();
	//
	//	char buf[1024];
	//	sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
	//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "VR_Init Failed", buf, NULL);
	//	return false;
	//}

	//TODO: add any xml loaders here:

//#define USE_SKELETAL_API

#ifdef USE_SKELETAL_API
	int gamepadIndex = 1;
	appShard.mXMLObjFactory.addLoader(new GHOpenVRSkeletalAvatarXMLLoader(hmdOrigin, mSystemServices.getPlatformServices().getIdFactory(), mSystemServices.getInputState(), gamepadIndex), 1, "hmdAvatarController");
#else
	const char* avatarAppID = "";
	uint64_t avatarUserID = 0;
	appShard.mXMLObjFactory.addLoader(new GHOculusAvatarXMLLoader(avatarAppID, avatarUserID, appShard.mXMLObjFactory, *mHMDOrigin, *mAvatarPoserFactory, 
																  *mRenderServices.getVBFactory(), *mRenderServices.getMaterialCallbackMgr(), mSystemServices.getPlatformServices().getIdFactory(),
																  mSystemServices.getInputState(), appShard.mProps, mSystemServices.getXMLSerializer(), 
																  mSystemServices.getEventMgr(), appShard.mEventMgr,
																  appShard.mResourceFactory),
									  1, "hmdAvatarController");
#endif

	mInputHandler = new GHOpenVRInputHandler(hmdOrigin, mOpenVR, mSystemServices.getInputState(), realTimeVal);
	mControllerMgr.add(mInputHandler);

	// make sure the hmd activation gets sent again in case somebody missed it.
	if (mOpenVR.hmdIsActive())
	{
		mSystemServices.getEventMgr().handleMessage(GHMessage(GHRenderProperties::M_HMDACTIVATED));
	}
}

#include "GHNullController.h"

GHController* GHOpenVRDX11HMDServices::createHMDTransformApplier(GHTransform& outTrans)
{
	return new GHOpenVRHMDTransformApplier(*mInputHandler, *mHMDOrigin, outTrans);
}
