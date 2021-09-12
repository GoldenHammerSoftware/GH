// Copyright Golden Hammer Software
#include "GHWin32SystemServices.h"
#include "GHInputState.h"
#include "GHPlatform/win32/GHWin32TimeCalculator.h"
#include "GHPlatform/GHFOpenFileOpener.h"
#include "GHPlatform/win32/GHWin32ThreadFactory.h"
#include "GHBaseIdentifiers.h"
#include "GHWin32MouseCapturer.h"
#include "GHNullIAPStoreFactory.h"
#include "GHAppShard.h"
#include "GHBaseIdentifiers.h"
#include "GHWin32URLOpenerTransitionXMLLoader.h"
#include "GHPlatform/win32/GHWinSockSocketMgr.h"
#include "GHWin32ExitAppTransitionXMLLoader.h"
#include "GHWin32Window.h"
//#include "GHDirectSoundDevice.h"
//#include "GHDirectSoundLoader.h"
#include "GHMetroSoundDevice.h"
#include "GHMetroSoundLoader.h"
#include "GHWin32SoundFinder.h"
#include "GHPlatform/win32/GHWin32PlatformServices.h"
#include "GHXInputRumbler.h"

GHWin32SystemServices::GHWin32SystemServices(GHWin32Window& window, GHControllerMgr& controllerManager)
: GHSystemServices(new GHWin32PlatformServices(window.getHWND()),
	new GHInputState(10, 1, 0, 3, 2, 2, 1),
	0, 
	new GHWin32MouseCapturer(window),
	new GHXInputRumbler(),
	&window)
, mSoundFinder(0)
{
	mSoundFinder = new GHWin32SoundFinder(getPlatformServices().getFileOpener());
	mSoundDevice = new GHMetroSoundDevice(mEventMgr, controllerManager, mSoundVolumeMgr, getPlatformServices().getTimeCalculator());
}

void GHWin32SystemServices::initAppShard(GHAppShard& appShard)
{
	GHSystemServices::initAppShard(appShard);

	appShard.mResourceFactory.addLoader(new GHMetroSoundLoader(*mSoundFinder, getPlatformServices().getThreadFactory()), 3, ".mp3", ".wav", ".m4a");

//	GHDirectSoundLoader* soundLoader = new GHDirectSoundLoader(*mFileOpener, *((GHDirectSoundDevice*)mSoundDevice));
//	appShard.mResourceFactory.addLoader(soundLoader, 1, ".wav");

	((GHXInputRumbler*)mRumbler)->setPropertyContainer(&appShard.mProps);

	GHWin32URLOpenerTransitionXMLLoader* urlTrans = new GHWin32URLOpenerTransitionXMLLoader();
	appShard.mXMLObjFactory.addLoader(urlTrans, 1, "openURL");

	GHWin32ExitAppTransitionXMLLoader* exitTrans = new GHWin32ExitAppTransitionXMLLoader();
	appShard.mXMLObjFactory.addLoader(exitTrans, 1, "exitApp");

	appShard.mProps.setProperty(GHBaseIdentifiers::FULLSCREENSUPPORTED, true);
}
