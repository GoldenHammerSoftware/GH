// Copyright 2010 Golden Hammer Software
#pragma once

class GHiOSSoundDevice;
class GHEventMgr;
class GHSoundVolumeMgr;

class GHiOSSoundDeviceFactory
{
public:
	GHiOSSoundDeviceFactory(GHEventMgr& eventMgr, GHSoundVolumeMgr& volumeMgr);

	GHiOSSoundDevice*	createDevice(void);
	
private:
	GHEventMgr& mEventMgr;
    GHSoundVolumeMgr& mVolumeMgr;
};
