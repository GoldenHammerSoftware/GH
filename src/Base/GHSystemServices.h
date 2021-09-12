// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHEventMgr.h"
#include "GHXMLSerializer.h"
#include "GHSoundVolumeMgr.h"
#include "GHPlatform/GHPlatformServices.h"
#include "GHUtils/GHUtilsServices.h"
#include <assert.h>

class GHInputState;
class GHAppShard;
class GHSoundDevice;
class GHMouseCapturer;
class GHXMLObjLoader;
class GHServices;
class GHRumbler;
class GHSocketMgr;
class GHWindow;

// stuff created by the platform and shared with apps.
class GHSystemServices
{
public:
    GHSystemServices(GHPlatformServices* platformServices, 
		GHInputState* inputState,
        GHSoundDevice* soundDevice,
        GHMouseCapturer* mouseCapturer,
		GHRumbler* rumbler, 
		GHWindow* window);
    virtual ~GHSystemServices(void);

    virtual void initAppShard(GHAppShard& appShard);

	//For adding app- and platform-specific stuff
	void addSubService(GHServices* service);

    GHPlatformServices& getPlatformServices(void) { assert(mPlatformServices); return *mPlatformServices; }
    GHUtilsServices& getUtilsServices(void) { return mUtilsServices; }
    GHEventMgr& getEventMgr(void) { return mEventMgr; }
    GHXMLSerializer& getXMLSerializer(void) { return mXMLSerializer; }
    GHInputState& getInputState(void) { assert(mInputState); return *mInputState; }
    GHSoundDevice& getSoundDevice(void) { assert(mSoundDevice); return *mSoundDevice; }
    GHSoundVolumeMgr& getSoundVolumeMgr(void) { return mSoundVolumeMgr; }
    GHMouseCapturer& getMouseCapturer(void) { assert(mMouseCapturer); return *mMouseCapturer; }
    GHRumbler& getRumbler(void) { assert(mRumbler); return *mRumbler; }
    GHWindow& getWindow(void) { assert(mWindow); return *mWindow; }

protected:
	GHUtilsServices mUtilsServices;
    GHEventMgr mEventMgr;
    GHXMLSerializer mXMLSerializer;
    GHSoundVolumeMgr mSoundVolumeMgr;
    GHPlatformServices* mPlatformServices;
    GHInputState* mInputState;
    GHSoundDevice* mSoundDevice;
    GHMouseCapturer* mMouseCapturer;
	GHRumbler* mRumbler;
	GHWindow* mWindow;

    std::vector<GHServices*> mSubServices;
};
