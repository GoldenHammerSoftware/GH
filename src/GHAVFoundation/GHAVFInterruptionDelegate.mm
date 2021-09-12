// Copyright 2010 Golden Hammer Software
#import "GHAVFInterruptionDelegate.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"

@implementation GHAVFInterruptionDelegate

@synthesize mMessageHandler;

- (void)audioPlayerBeginInterruption:(AVAudioPlayer *)player
{
    if (mMessageHandler)
    {
        mMessageHandler->handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT, 0));
    }
}


- (void)audioPlayerEndInterruption:(AVAudioPlayer *)player
{
    if (mMessageHandler)
    {
        mMessageHandler->handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT, 0));
    }
}

@end
