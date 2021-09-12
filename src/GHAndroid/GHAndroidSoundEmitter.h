// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"
#include "GHUtils/GHMessageHandler.h"
#include <jni.h>

class GHAndroidSoundPoolHandle;
class GHAndroidSoundPool;
class GHEventMgr;

class GHAndroidSoundEmitter : public GHSoundEmitter
{
public:
	GHAndroidSoundEmitter(GHAndroidSoundPoolHandle& soundHandle, GHAndroidSoundPool& mediaPlayer, GHEventMgr& eventMgr, 
		unsigned int emitterGUID, const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
	~GHAndroidSoundEmitter(void);

	virtual void play(void);
	virtual void pause(void);
	virtual void stop(void);

	virtual void setPosition(const GHPoint3& pos);
	virtual void setVelocity(const GHPoint3& pos);
	virtual void setLooping(bool looping);
	virtual void setPitch(float pitch);
	virtual bool isPlaying(void) const;
    virtual bool isLooping(void) const;
	virtual void seek(float t);

    void handlePauseInterrupt(void);
    void handleUnpauseInterrupt(void);
    
	// todo
	virtual void setIsPositional(bool isPositional) {}
	virtual bool isPositional(void) const { return false; }

private:
    virtual void applyGain(float gain);
    
private:
    class MessageListener : public GHMessageHandler
    {
    public:
        MessageListener(GHEventMgr& eventMgr, GHAndroidSoundEmitter& parent) : mEventMgr(eventMgr), mParent(parent), mIsSubscribed(false) { }
        ~MessageListener(void);
        
        virtual void handleMessage(const GHMessage& message);
        
        void unsubscribeFromInterrupt(void);
        void subscribeToInterrupt(void);
        
    private:
        GHEventMgr& mEventMgr;
        GHAndroidSoundEmitter& mParent;
        bool mIsSubscribed;
    };
    MessageListener mMessageListener;
    
private:
    GHAndroidSoundPool& mMediaPlayer;
    GHAndroidSoundPoolHandle& mSoundHandle;
    jobject mMediaPlayerObj;
    unsigned int mEmitterGUID;
    bool mIsLooping;
    bool mStartedPlaying;
    bool mWasInterrupted;
};
