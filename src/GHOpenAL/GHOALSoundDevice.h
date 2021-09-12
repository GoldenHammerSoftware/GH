// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundDevice.h"
#include "GHOALInclude.h"

class GHOALSoundHandleMgr;
class GHEventMgr;
class GHMessageHandler;
class GHTransition;
class GHSoundVolumeMgr;

class GHOALSoundDevice : public GHSoundDevice
{
public:
	GHOALSoundDevice(GHEventMgr& eventMgr, 
                     GHTransition* platformSoundPauseTransition,
                     GHSoundVolumeMgr& volumeMgr);
	~GHOALSoundDevice(void);
	
	virtual void setListenerPosition(const GHPoint3& pos);
	virtual GHSoundEmitter* createEmitter(GHSoundHandle& sound, const GHIdentifier& category);
    // todo
    virtual void enable3DSound(bool enable) {}
    virtual void setListenerOrientation(const GHTransform& orientation) {}
    virtual void setListenerVelocity(const GHPoint3& velocity) {}

    void handlePause(void);
    void handleUnpause(void);
    
    //mSoundHandleMgr is owned by the GHOALSoundDevice because it needs to be constructed after
    // the OAL system is initialized.
    GHOALSoundHandleMgr& getSoundHandleMgr(void) { return *mSoundHandleMgr; }

protected:
	void destroyContext(void);
	void createContext(void);
	
protected:
	ALCcontext* mContext;
	ALCdevice* mDevice;
	GHOALSoundHandleMgr* mSoundHandleMgr;
	GHEventMgr& mEventManager;
	GHTransition* mPlatformSoundPauseTransition;
    GHMessageHandler* mInterruptHandler;
    GHSoundVolumeMgr& mVolumeMgr;
};
