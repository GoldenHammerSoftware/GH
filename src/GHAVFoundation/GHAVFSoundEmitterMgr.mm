// Copyright 2010 Golden Hammer Software
#include "GHAVFSoundEmitterMgr.h"
#include "GHSoundEmitter.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"

GHAVFSoundEmitterMgr::GHAVFSoundEmitterMgr(GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
, mCurrentlyPlayingEmitter(0)
, mInterruptedEmitter(0)
, mCurrentlyInterrupted(false)
{
	mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
    mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
}

GHAVFSoundEmitterMgr::~GHAVFSoundEmitterMgr(void)
{
	mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
    mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
}

bool GHAVFSoundEmitterMgr::registerPlayingSound(GHSoundEmitter* emitter)
{
	if (mCurrentlyInterrupted) {
		mInterruptedEmitter = emitter;
	}
	else if (mCurrentlyPlayingEmitter && mCurrentlyPlayingEmitter != emitter) {
		mCurrentlyPlayingEmitter->stop();
	}
	mCurrentlyPlayingEmitter = emitter;
	
	return !mCurrentlyInterrupted;
}

void GHAVFSoundEmitterMgr::unregisterPlayingSound(GHSoundEmitter* emitter)
{
	if (emitter == mCurrentlyPlayingEmitter) {
		mCurrentlyPlayingEmitter = 0;
	}
}

void GHAVFSoundEmitterMgr::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
    {
        mCurrentlyInterrupted = true;
        mInterruptedEmitter = mCurrentlyPlayingEmitter;
        if(mInterruptedEmitter)
        {
            mInterruptedEmitter->stop();
        }
    }
    else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
        mCurrentlyInterrupted = false;
        if(mInterruptedEmitter)
        {
            mInterruptedEmitter->play();
        }
        mInterruptedEmitter = 0;
    }
}
