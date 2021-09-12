// Copyright 2010 Golden Hammer Software
#include "GHMetroSystemServices.h"
#include "GHInputState.h"
#include "GHMetroSoundDevice.h"
#include "GHMetroMouseCapturer.h"
#include "GHNullMultiplayerFactory.h"
#include "GHMetroSoundLoader.h"
#include "GHAppShard.h"
#include "GHMetroURLOpenerTransitionLoader.h"
#include "GHNullStatTrackerFactory.h"
#include "GHXInputRumbler.h"
#include "GHBaseIdentifiers.h"
#include "GHMetroFileTokenListener.h"
#include "GHNullWindow.h"

#include "GHPlatform/uwp/GHMetroSocketMgr.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHPlatform/uwp/GHMetroTimeCalculator.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"
#include "GHPlatform/uwp/GHMetroThreadFactory.h"
#include "GHPlatform/uwp/GHMetroFilePicker.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"
#include "GHPlatform/uwp/GHMetroPlatformServices.h"

GHMetroSystemServices::GHMetroSystemServices(GHControllerMgr& controllerManager)
	: GHSystemServices(new GHMetroPlatformServices(),
		new GHInputState(sMaxPointers,1,1,4,2,2,1),
		0, // sound
		new GHMetroMouseCapturer(),
		new GHXInputRumbler(),
		new GHNullWindow()
		)
	, mSoundFinder((GHMetroFileOpener&)mPlatformServices->getFileOpener())
{
	// mouse 0 is always active
	mInputState->handlePointerActive(0, true);
	mSoundDevice = new GHMetroSoundDevice(mEventMgr, controllerManager, mSoundVolumeMgr, mPlatformServices->getTimeCalculator());
}

void GHMetroSystemServices::initAppShard(GHAppShard& appShard)
{
	GHSystemServices::initAppShard(appShard);
	appShard.mResourceFactory.addLoader(new GHMetroSoundLoader(mSoundFinder, mPlatformServices->getThreadFactory()), 3, ".mp3", ".wav", ".m4a");

	GHMetroURLOpenerTransitionLoader* urlOpener = new GHMetroURLOpenerTransitionLoader;
	appShard.mXMLObjFactory.addLoader(urlOpener, 1, "openURL");

	((GHXInputRumbler*)mRumbler)->setPropertyContainer(&appShard.mProps);

	GHMetroFileTokenListener* ftl = new GHMetroFileTokenListener(appShard.mEventMgr);
	appShard.addOwnedItem(new GHTypedDeletionHandle<GHMetroFileTokenListener>(ftl));
}
