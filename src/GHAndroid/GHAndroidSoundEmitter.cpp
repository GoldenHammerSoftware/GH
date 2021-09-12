// Copyright 2010 Golden Hammer Software
#include "GHAndroidSoundEmitter.h"
#include "GHAndroidSoundPoolHandle.h"
#include "GHAndroidSoundPool.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHProfiler.h"
#include "GHPlatform/GHDebugMessage.h"

GHAndroidSoundEmitter::GHAndroidSoundEmitter(GHAndroidSoundPoolHandle& soundHandle, GHAndroidSoundPool& mediaPlayer, GHEventMgr& eventMgr, unsigned int emitterGUID, const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: GHSoundEmitter(category, volumeMgr)
, mMessageListener(eventMgr, *this)
, mSoundHandle(soundHandle)
, mMediaPlayer(mediaPlayer)
, mMediaPlayerObj(0)
, mEmitterGUID(emitterGUID)
, mIsLooping(false)
, mStartedPlaying(false)
, mWasInterrupted(false)
{
    GHPROFILESCOPE("GHAndroidSoundEmitter contructor", GHString::CHT_REFERENCE);
    mSoundHandle.acquire();
    mMediaPlayerObj = mMediaPlayer.createSound(mSoundHandle.mFilename.getChars());
    updateGain();
}

GHAndroidSoundEmitter::~GHAndroidSoundEmitter(void)
{
    mMediaPlayer.releaseSound(mMediaPlayerObj);
    if (mIsLooping) {
        mMessageListener.unsubscribeFromInterrupt();
    }
    mSoundHandle.release();
}

void GHAndroidSoundEmitter::play(void)
{
    mMediaPlayer.start(mMediaPlayerObj);
	mStartedPlaying = true;
}

void GHAndroidSoundEmitter::pause(void)
{
    if (mStartedPlaying) {
		mMediaPlayer.pause(mMediaPlayerObj);
        mStartedPlaying = false;
	}
}

void GHAndroidSoundEmitter::stop(void)
{
	if (mStartedPlaying) {
		mMediaPlayer.stop(mMediaPlayerObj);
		mStartedPlaying = false;
	}
}

void GHAndroidSoundEmitter::setPosition(const GHPoint3& pos)
{

}

void GHAndroidSoundEmitter::setVelocity(const GHPoint3& pos)
{

}

void GHAndroidSoundEmitter::setLooping(bool looping)
{
    if (looping != mIsLooping){
		if(looping){
            mMessageListener.subscribeToInterrupt();
		}
		else {
            mMessageListener.unsubscribeFromInterrupt();
		}
		mMediaPlayer.setLooping(mMediaPlayerObj, looping);
	}
	mIsLooping = looping;
}

void GHAndroidSoundEmitter::setPitch(float pitch)
{
    mMediaPlayer.setPitch(mMediaPlayerObj, pitch);
}

void GHAndroidSoundEmitter::applyGain(float gain)
{
    mMediaPlayer.setVolume(mMediaPlayerObj, gain);
}

bool GHAndroidSoundEmitter::isPlaying(void) const 
{
	return mMediaPlayer.isPlaying(mMediaPlayerObj);
}

bool GHAndroidSoundEmitter::isLooping(void) const
{
    return mIsLooping;
}

void GHAndroidSoundEmitter::handlePauseInterrupt(void)
{
	if (mStartedPlaying) {
		pause();
        mWasInterrupted = true;
	}
}

void GHAndroidSoundEmitter::handleUnpauseInterrupt(void)
{
	if (mWasInterrupted) {
        // avoiding a bug in media player java
        //play();
        mWasInterrupted = false;
	}
}

GHAndroidSoundEmitter::MessageListener::~MessageListener(void)
{
    unsubscribeFromInterrupt();
}

void GHAndroidSoundEmitter::MessageListener::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
    {
        mParent.handlePauseInterrupt();
    }
    else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
        mParent.handleUnpauseInterrupt();
    }
}

void GHAndroidSoundEmitter::MessageListener::unsubscribeFromInterrupt(void)
{
    if (mIsSubscribed)
    {
        mEventMgr.unregisterListener(GHMessageTypes::PAUSEINTERRUPT, *this);
        mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
        mIsSubscribed = false;
    }
}

void GHAndroidSoundEmitter::MessageListener::subscribeToInterrupt(void)
{
    if (!mIsSubscribed)
    {
        mEventMgr.registerListener(GHMessageTypes::PAUSEINTERRUPT, *this);
        mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *this);
        mIsSubscribed = true;
    }
}

void GHAndroidSoundEmitter::seek(float t)
{
	GHDebugMessage::outputString("Unimplemented GHAndroidSoundEmitter::seek");
}