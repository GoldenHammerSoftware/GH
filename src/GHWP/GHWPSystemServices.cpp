// Copyright 2010 Golden Hammer Software
#include "GHWPSystemServices.h"
#include "GHInputState.h"
#include "GHMetroTimeCalculator.h"
#include "GHMetroFileOpener.h"
#include "GHMetroThreadFactory.h"
#include "GHMetroSoundDevice.h"
#include "GHNullMouseCapturer.h"
#include "GHAppShard.h"
#include "GHMetroURLOpenerTransitionLoader.h"
#include "GHWPExitAppTransitionXMLLoader.h"
#include "GHWPSoundLoader.h"

#define ENUMHEADER "GHWPIdentifiers.h"
#include "GHEnumInclude.h"

GHWPSystemServices::GHWPSystemServices(GHControllerMgr& systemControllerMgr)
: GHSystemServices(new GHInputState(sMaxPointers,1,1,0,0),
					new GHMetroTimeCalculator(),
					new GHMetroFileOpener(L"\\Assets\\Game\\", L"\\"),
					new GHMetroThreadFactory(),
					0, // sound
					new GHNullMouseCapturer())
, mExitOnBack(false)
{
	GHWPIdentifiers::generateIdentifiers(mHashTable);
	mSoundDevice = new GHMetroSoundDevice(mEventMgr, systemControllerMgr, mSoundVolumeMgr, *mTimeCalc);
} 


void GHWPSystemServices::initAppShard(GHAppShard& appShard)
{
	GHSystemServices::initAppShard(appShard);

	GHMetroURLOpenerTransitionLoader* urlOpener = new GHMetroURLOpenerTransitionLoader;
	appShard.mXMLObjFactory.addLoader(urlOpener, 1, "openURL");

	GHWPExitAppTransitionXMLLoader* exitLoader = new GHWPExitAppTransitionXMLLoader(mExitOnBack);
	appShard.mXMLObjFactory.addLoader(exitLoader, 1, "exitOnBack");

	appShard.mResourceFactory.addLoader(new GHWPSoundLoader(*mFileOpener), 1, ".wav");
}
