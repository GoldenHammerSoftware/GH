// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHSoundEmitter;
class GHEventMgr;

//Only one sound can be played at a time through this system.
class GHAVFSoundEmitterMgr : public GHMessageHandler
{
public:
	GHAVFSoundEmitterMgr(GHEventMgr& eventMgr);
	~GHAVFSoundEmitterMgr(void);
	bool registerPlayingSound(GHSoundEmitter* emitter);
	void unregisterPlayingSound(GHSoundEmitter* emitter);
	
    virtual void handleMessage(const GHMessage& message);
	
private:
	GHEventMgr& mEventMgr;
	GHSoundEmitter* mCurrentlyPlayingEmitter;
	GHSoundEmitter* mInterruptedEmitter;
	bool mCurrentlyInterrupted;
};
