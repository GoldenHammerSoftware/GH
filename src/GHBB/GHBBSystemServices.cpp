// Copyright 2010 Golden Hammer Software
#include "GHBBSystemServices.h"
#include "GHInputState.h"
#include "GHBBTimeCalculator.h"
#include "GHBBFileOpener.h"
#include "GHNullThreadFactory.h"
#include "GHNullMouseCapturer.h"
#include "GHNullSoundDevice.h"
#include "GHAppShard.h"
#include "GHDebugMessage.h"
#include "GHOALSoundDevice.h"
#include "GHALUTSoundLoader.h"
#include <alut.h>
#include "GHBBIdentifiers.h"

#define ENUMHEADER "GHBBIdentifiers.h"
#include "GHEnumInclude.h"

GHBBSystemServices::GHBBSystemServices(void)
: GHSystemServices(new GHInputState(10,1,1),
                   new GHBBTimeCalculator(),
                   new GHBBFileOpener(),
                   new GHNullThreadFactory(),
                   0, //sound device
                   new GHNullMouseCapturer())
, mOALSoundDevice(0)
{
	//must be called before OpenAL calls in the GHOALSoundDevice ctor
	if (alutInit(0, 0))
	{
		mOALSoundDevice = new GHOALSoundDevice(mEventMgr, 0, mSoundVolumeMgr);
		mSoundDevice = mOALSoundDevice;
	}
	else
	{
		GHDebugMessage::outputString("alutInit failed. Creating null sound device");
		mSoundDevice = new GHNullSoundDevice(mSoundVolumeMgr);
	}

	GHBBIdentifiers::generateIdentifiers(mHashTable);

    GHDebugMessage::init(mOutputPipe);
    GHDebugMessage::outputString("Created GHBBSystemServices.");
}

void GHBBSystemServices::initAppShard(GHAppShard& appShard)
{
    GHSystemServices::initAppShard(appShard);

    if (mOALSoundDevice)
    {
    	GHALUTSoundLoader* alutSoundLoader = new GHALUTSoundLoader(mOALSoundDevice->getSoundHandleMgr(), GHString("app/native/", GHString::CHT_REFERENCE));
    	appShard.mResourceFactory.addLoader(alutSoundLoader, 3, ".wav", ".aiff", ".m4a");
    }
}

