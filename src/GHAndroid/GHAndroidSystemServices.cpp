// Copyright 2010 Golden Hammer Software
#include "GHAndroidSystemServices.h"
#include "GHInputState.h"
#include "GHNullMouseCapturer.h"
#include "GHAppShard.h"
#include "GHPlatform/GHDebugMessage.h" 
#include "GHAndroidURLOpenerTransitionLoader.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHNullRumbler.h"
#include "GHAndroidExitHandlerXMLLoader.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include "GHPlatform/android/GHAndroidPlatformServices.h"
#include "GHAndroidKeyboardTransitionXMLLoader.h"

#ifdef GHOPENSL
#include "GHOpenSLDevice.h"
#include "GHOpenSLLoader.h"
#else
#include "GHAndroidSoundDevice.h"
#include "GHAndroidSoundLoader.h"
#endif

#define ENUMHEADER "GHAndroidIdentifiers.h"
#include "GHString/GHEnumInclude.h"

GHAndroidSystemServices::GHAndroidSystemServices(GHJNIMgr& jniMgr, jobject engineInterface, std::vector<GHString>&& saveFileDirectories, jobject jAssetMgr)
: GHSystemServices(
	new GHAndroidPlatformServices(jniMgr, engineInterface, std::move(saveFileDirectories), jAssetMgr),
	new GHInputState(10, 1, 0, 3, 2, 2, 1),
    0,
    new GHNullMouseCapturer(),
    new GHNullRumbler(),
	0)
, mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mAssetMgr(jAssetMgr)
{
    GHAndroidIdentifiers::generateIdentifiers(mPlatformServices->getIdFactory());
    GHDebugMessage::outputString("Created GHAndroidSystemServices.");

#ifdef GHOPENSL
	GHDebugMessage::outputString("Creating new GHOpenSLDevice");
	mSoundDevice = new GHOpenSLDevice(mSoundVolumeMgr);
#else
	mSoundDevice = mAndroidSoundDevice = new GHAndroidSoundDevice(mJNIMgr, engineInterface, mEventMgr, mSoundVolumeMgr);
#endif
}

void GHAndroidSystemServices::initAppShard(GHAppShard& appShard)
{
    GHSystemServices::initAppShard(appShard);

#ifdef GHOPENSL
	GHOpenSLLoader* soundLoader = new GHOpenSLLoader(mJNIMgr, mAssetMgr);
	appShard.mResourceFactory.addLoader(soundLoader, 4, ".mp3", ".wav", ".aiff", ".m4a"); 
#else
	GHAndroidSoundLoader* soundLoader = new GHAndroidSoundLoader(mAndroidSoundDevice->getMediaPlayer());
	appShard.mResourceFactory.addLoader(soundLoader, 4, ".mp3", ".wav", ".aiff", ".m4a");
#endif

    GHAndroidURLOpenerTransitionLoader* urlLoader = new GHAndroidURLOpenerTransitionLoader(mJNIMgr, mEngineInterface);
    appShard.mXMLObjFactory.addLoader(urlLoader, 1, "openURL");

	GHAndroidExitHandlerXMLLoader* exitLoader = new GHAndroidExitHandlerXMLLoader(mJNIMgr, mEngineInterface);
	appShard.mXMLObjFactory.addLoader(exitLoader, 1, "exitApp");

	GHAndroidKeyboardTransitionXMLLoader* keyboardTrans = new GHAndroidKeyboardTransitionXMLLoader(mJNIMgr, mEngineInterface);
	appShard.mXMLObjFactory.addLoader(keyboardTrans, 1, "showKeyboard");
}

