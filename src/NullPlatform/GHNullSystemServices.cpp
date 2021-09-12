// Copyright Golden Hammer Software
#include "GHNullSystemServices.h"
#include "GHInputState.h"
#include "GHNullSoundDevice.h"
#include "GHNullMouseCapturer.h"
#include "GHNullIAPStoreFactory.h"
#include "GHNullRumbler.h"
#include "GHPlatform/null/GHNullPlatformServices.h"
#include "GHNullWindow.h"

GHNullSystemServices::GHNullSystemServices(void)
: GHSystemServices(new GHNullPlatformServices(),
	new GHInputState(10,1,0,1,2,0,0),
    new GHNullSoundDevice(mSoundVolumeMgr),
    new GHNullMouseCapturer(),
	new GHNullRumbler(),
	new GHNullWindow())
{
}
