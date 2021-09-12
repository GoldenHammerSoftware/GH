// Copyright 2010 Golden Hammer Software
#include "GHCocoaSystemServices.h"
#include "GHPlatform/osx/GHCocoaPlatformServices.h"
#include "GHInputState.h"
#include "GHAppShard.h"
#include "GHAppleOALSoundLoader.h"
#include "GHOALSoundDevice.h"
#include "GHCocoaMouseCapturer.h"
#include "GHCocoaFileNameCreator.h"
#include "GHCocoaURLOpenerTransitionXMLLoader.h"
#include "GHGameCenterMultiplayerFactory.h"
#include "GHAppleIdentifiers.h"
#include "GHCocoaTouchpadMouseSeparatorXMLLoader.h"
#include "GHNullRumbler.h"

#define ENUMHEADER "GHAppleIdentifiers.h"
#include "GHEnumInclude.h"


GHCocoaSystemServices::GHCocoaSystemServices(GHCocoaView* view)
: GHSystemServices(new GHCocoaPlatformServices(),
                   new GHInputState(10,1,0,1,2,0,0),
                   0,
                   new GHCocoaMouseCapturer(view),
                   new GHNullRumbler(),
                   0)
, mCocoaView(view)
{
    //can't pass this to ctor because it needs to be initialized with mEventMgr,
    // which isn't constructed until now.
    mSoundDevice = new GHOALSoundDevice(mEventMgr, 0, mSoundVolumeMgr);
    
    GHAppleIdentifiers::generateIdentifiers(mPlatformServices->getIdFactory());
}

void GHCocoaSystemServices::initAppShard(GHAppShard& appShard)
{
    GHSystemServices::initAppShard(appShard);
    
    appShard.mResourceFactory.addLoader(new GHAppleOALSoundLoader(((GHOALSoundDevice*)mSoundDevice)->getSoundHandleMgr(),
                                                                  new GHCocoaFileNameCreator), 
                                        3, 
                                        ".wav",
                                        ".aiff",
                                        ".m4a");
    
    GHCocoaURLOpenerTransitionXMLLoader* urlLoader = new GHCocoaURLOpenerTransitionXMLLoader();
    appShard.mXMLObjFactory.addLoader(urlLoader, 1, "openURL");
    
    GHCocoaTouchpadMouseSeparatorXMLLoader* touchpadMouseSeparatorLoader = new GHCocoaTouchpadMouseSeparatorXMLLoader(mCocoaView);
    appShard.mXMLObjFactory.addLoader(touchpadMouseSeparatorLoader, 1, "touchpadMouseSeparator");
}
