// Copyright 2010 Golden Hammer Software
#include "GHiOSSystemServices.h"
#include "GHInputState.h"
#include "GHiOSSoundDeviceFactory.h"
#include "GHAppShard.h"
#include "GHAppleOALSoundLoader.h"
#include "GHiOSSoundDevice.h"
#include "GHiOSFilenameCreator.h"
#include "GHiOSAVFSoundLoader.h"
#include "GHiOSURLOpenerTransitonXMLLoader.h"
#include "GHiOSIdentifiers.h"
#include "GHAppleIdentifiers.h"
//#include "GHiOSRumbler.h"
// what happened to ghios rumbler?
#include "GHNullRumbler.h"
#include "GHNullMouseCapturer.h"
#import <UIKit/UIKit.h>
#include "GHiOSPauseInterruptHandler.h"
#include "GHPlatform/ios/GHiOSPlatformServices.h"
#include "GHNullWindow.h"

#define ENUMHEADER "GHiOSIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHAppleIdentifiers.h"
#include "GHString/GHEnumInclude.h"

GHiOSSystemServices::GHiOSSystemServices(GHiOSViewBehavior& viewBehavior)
: GHSystemServices(new GHiOSPlatformServices(),
                   new GHInputState(10,1,1,1,2,0,0),
                   0,
                   new GHNullMouseCapturer(),
                   new GHNullRumbler(),//GHiOSRumbler(),
                   new GHNullWindow())
, mViewBehavior(viewBehavior)
{
    GHiOSIdentifiers::generateIdentifiers(mPlatformServices->getIdFactory());
    GHAppleIdentifiers::generateIdentifiers(mPlatformServices->getIdFactory());
    
    //can't pass this to ctor because it needs to be initialized with mEventMgr,
    // which isn't constructed until now.
    GHiOSSoundDeviceFactory deviceFactory(mEventMgr, mSoundVolumeMgr);
    miOSSoundDevice = deviceFactory.createDevice();
    mSoundDevice = miOSSoundDevice;
    
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        mPlatformServices->getFileOpener().addResourcePath("iPad-");
        mPlatformServices->getFileOpener().addResourcePath("Tablet-");
    }
    else
    {
        mPlatformServices->getFileOpener().addResourcePath("Phone-");
    }
    mPauseInterruptHandler = 0;
}

GHiOSSystemServices::~GHiOSSystemServices(void)
{
    delete mPauseInterruptHandler;
}

void GHiOSSystemServices::initAppShard(GHAppShard& appShard)
{
    GHSystemServices::initAppShard(appShard);

    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        GHIdentifier ipadProp = mPlatformServices->getIdFactory().generateHash("GP_IPAD");
        appShard.mProps.setProperty(ipadProp, true);
    }

    appShard.mResourceFactory.addLoader(new GHAppleOALSoundLoader(miOSSoundDevice->getOALDevice().getSoundHandleMgr(),
                                                                new GHiOSFilenameCreator),
                                                                3, 
                                                                ".wav",
                                                                ".aiff",
                                                                ".m4a");

    if (miOSSoundDevice->canPlayMusic())
    {
        appShard.mResourceFactory.addLoader(new GHiOSAVFSoundLoader(), 1, ".mp3");
    }
    
    GHiOSURLOpenerTransitonXMLLoader* urlTrans = new GHiOSURLOpenerTransitonXMLLoader;
    appShard.mXMLObjFactory.addLoader(urlTrans, 1, "openURL");
    
    delete mPauseInterruptHandler;
    mPauseInterruptHandler = new GHiOSPauseInterruptHandler(appShard.mEventMgr, mEventMgr, mViewBehavior);
}
