// Copyright Golden Hammer Software
#include "GHSystemServices.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHSoundDevice.h"
#include "GHInputState.h"
#include "GHMouseCapturer.h"
#include "GHPlatform/GHFilePicker.h"
#include "GHPlatform/GHSocketMgr.h"
#include "GHWindow.h"
#include "GHBaseXMLInitializer.h"
#include "GHAppShard.h"
#include "GHUtils/GHPropertySetMessageHandler.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHServices.h"
#include "GHUtils/GHRunnableMessageHandler.h"
#include "GHRumbler.h"
#include "GHWindowListener.h"
#include "GHMath/GHMathDebugPrint.h"
#include "GHUtils/GHUtilsMessageTypes.h"

#define ENUMHEADER "GHMessageTypes.h" 
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHKeyDef.h" 
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHBaseIdentifiers.h" 
#include "GHString/GHEnumInclude.h"

GHSystemServices::GHSystemServices(GHPlatformServices* platformServices, 
	GHInputState* inputState,
    GHSoundDevice* soundDevice,
    GHMouseCapturer* mouseCapturer,
	GHRumbler* rumbler,
	GHWindow* window)
: mPlatformServices(platformServices)
, mUtilsServices(*platformServices)
, mSoundDevice(soundDevice)
, mMouseCapturer(mouseCapturer)
, mInputState(inputState)
, mXMLSerializer(platformServices->getFileOpener())
, mRumbler(rumbler)
, mWindow(window)
{
	assert(platformServices);

    GHMessageTypes::generateIdentifiers(getPlatformServices().getIdFactory());
    GHBaseIdentifiers::generateIdentifiers(getPlatformServices().getIdFactory());
    GHKeyDef::generateEnum(getPlatformServices().getEnumStore());

    GHMathDebugPrint::PrintFunc = GHDebugMessage::outputString;
}

GHSystemServices::~GHSystemServices(void)
{
	size_t numSubServices = mSubServices.size();
	for (size_t i = 0; i < numSubServices; ++i)
	{
		delete mSubServices[i];
	}

	delete mRumbler;
    delete mMouseCapturer;
    delete mSoundDevice;
    delete mInputState;

	delete mPlatformServices;
}

void GHSystemServices::initAppShard(GHAppShard& appShard)
{
    // todo: attach the shard's event mgr to the global one.
    
    GHBaseXMLInitializer::initXMLLoaders(appShard, *this);

    GHPropertySetMessageHandler* propSetListener = new GHPropertySetMessageHandler(appShard.mProps, GHMessageTypes::SETAPPPROPERTY, appShard.mEventMgr);
    appShard.addOwnedItem(new GHTypedDeletionHandle<GHPropertySetMessageHandler>(propSetListener));

    GHRunnableMessageHandler* runnableHandler = new GHRunnableMessageHandler;
    appShard.mEventMgr.registerListener(GHUtilsMessageTypes::EXECRUNNABLE, *runnableHandler);
    appShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GHRunnableMessageHandler>(runnableHandler));
    
	size_t numSubServices = mSubServices.size();
	for (size_t i = 0; i < numSubServices; ++i)
	{
		mSubServices[i]->initAppShard(appShard);
	}

	if (mWindow)
	{
		GHWindowListener* winListener = new GHWindowListener(*mWindow, appShard.mProps, appShard.mEventMgr);
		appShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GHWindowListener>(winListener));
	}
}

void GHSystemServices::addSubService(GHServices* service)
{
	mSubServices.push_back(service);
}

