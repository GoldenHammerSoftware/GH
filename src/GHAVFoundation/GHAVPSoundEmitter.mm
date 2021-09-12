// Copyright 2010 Golden Hammer Software
#include "GHAVPSoundEmitter.h"
#include <AVFoundation/AVFoundation.h>
#include "GHAVPSoundHandle.h"
#include "GHAVFSoundEmitterMgr.h"
#import "UIKit/UIKit.h"

@implementation GHAVPNotificationObserver

- (void)registerForEndNotification:(GHAVPSoundEmitter*)emitter
{
    mEmitter = emitter;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(playerItemDidReachEnd:)
                                                 name:AVPlayerItemDidPlayToEndTimeNotification
                                               object:[emitter->getSoundHandle().getAudioPlayer() currentItem]];
}
- (void)unregiterForEndNotification
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:AVPlayerItemDidPlayToEndTimeNotification object:[mEmitter->getSoundHandle().getAudioPlayer() currentItem]];
}
- (void)playerItemDidReachEnd:(NSNotification *)notification
{
    if (!mEmitter->isLooping())
    {
        return;
    }
    mEmitter->stop();
    mEmitter->play();
}

@end

GHAVPSoundEmitter::GHAVPSoundEmitter(GHAVPSoundHandle& sound, GHAVFSoundEmitterMgr& emitterMgr,
                                     const GHIdentifier& category, GHSoundVolumeMgr& volumeMgr)
: GHSoundEmitter(category, volumeMgr)
, mEmitterMgr(emitterMgr)
, mSound(sound)
, mIsLooping(false)
, mLoopingMonitor([GHAVPNotificationObserver alloc])
{
    mSound.acquire();
    updateGain();
    [mLoopingMonitor registerForEndNotification:this];
}

GHAVPSoundEmitter::~GHAVPSoundEmitter(void)
{
    mEmitterMgr.unregisterPlayingSound(this);
    [mLoopingMonitor unregiterForEndNotification];
    mSound.release();
}

void GHAVPSoundEmitter::play(void)
{
    if (mEmitterMgr.registerPlayingSound(this))
    {
        [mSound.getAudioPlayer() play];
    }
}

void GHAVPSoundEmitter::pause(void)
{
    [mSound.getAudioPlayer() pause];
}

void GHAVPSoundEmitter::stop(void)
{
    [mSound.getAudioPlayer() pause];
    CMTime newTime = CMTimeMakeWithSeconds(0, 1);
    [mSound.getAudioPlayer() seekToTime:newTime];
    mEmitterMgr.unregisterPlayingSound(this);
}

void GHAVPSoundEmitter::setPosition(const GHPoint3& pos)
{
}

void GHAVPSoundEmitter::setVelocity(const GHPoint3& pos)
{
}

void GHAVPSoundEmitter::setLooping(bool looping)
{
    mIsLooping = looping;
}

void GHAVPSoundEmitter::setPitch(float pitch)
{
}

void GHAVPSoundEmitter::seek(float t)
{
    //todo
}

void GHAVPSoundEmitter::applyGain(float gain)
{
    NSString *version = [[UIDevice currentDevice] systemVersion];
    BOOL isAtLeastiOS7 = [version floatValue] >= 7.0;
    if (isAtLeastiOS7)
    {
        [mSound.getAudioPlayer() setVolume:gain];
    }
    else
    {
        NSArray *audioTracks = [mSound.getAsset() tracksWithMediaType:AVMediaTypeAudio];
        
        NSMutableArray *allAudioParams = [NSMutableArray array];
        for (AVAssetTrack *track in audioTracks) {
            AVMutableAudioMixInputParameters *audioInputParams =
            [AVMutableAudioMixInputParameters audioMixInputParameters];
            [audioInputParams setVolume:gain atTime:kCMTimeZero];
            [audioInputParams setTrackID:[track trackID]];
            [allAudioParams addObject:audioInputParams];
        }
        
        AVMutableAudioMix *audioMix = [AVMutableAudioMix audioMix];
        [audioMix setInputParameters:allAudioParams];
        
        [mSound.getPlayerItem() setAudioMix:audioMix];
    }
}

bool GHAVPSoundEmitter::isPlaying(void) const
{
    if ([mSound.getAudioPlayer() rate] == 0)
    {
        return false;
    }
    if ([mSound.getAudioPlayer() error] != nil)
    {
        return false;
    }
    return true;
}

bool GHAVPSoundEmitter::isLooping(void) const
{
    return mIsLooping;
}
