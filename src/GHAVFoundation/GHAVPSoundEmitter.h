// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSoundEmitter.h"

class GHAVPSoundHandle;
class GHAVFSoundEmitterMgr;
class GHAVPSoundEmitter;

// observer for hooking into the end notification for looping AVPlayer
@interface GHAVPNotificationObserver : NSObject {
    GHAVPSoundEmitter* mEmitter;
}
- (void)registerForEndNotification:(GHAVPSoundEmitter*)emitter;
- (void)unregiterForEndNotification;
- (void)playerItemDidReachEnd:(NSNotification *)notification;
@end

// An AVPlayer class that plays a sound buffer at a location.
class GHAVPSoundEmitter : public GHSoundEmitter
{
public:
    GHAVPSoundEmitter(GHAVPSoundHandle& sound, GHAVFSoundEmitterMgr& soundEmitterMgr,
                      const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr);
    ~GHAVPSoundEmitter(void);
    
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
    
    // todo:
    virtual void setIsPositional(bool isPositional) {}
    virtual bool isPositional(void) const { return false; }
    
    GHAVPSoundHandle& getSoundHandle(void) { return mSound; }
    
private:
    virtual void applyGain(float gain);
    
protected:
    GHAVFSoundEmitterMgr& mEmitterMgr;
    GHAVPSoundHandle& mSound;
    bool mIsLooping;
    GHAVPNotificationObserver* mLoopingMonitor;
};
