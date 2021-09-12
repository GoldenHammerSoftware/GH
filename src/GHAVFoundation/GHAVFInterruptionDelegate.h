// Copyright 2010 Golden Hammer Software
#pragma once

#include <AVFoundation/AVFoundation.h>

class GHMessageHandler;

@interface GHAVFInterruptionDelegate : NSObject<AVAudioPlayerDelegate> {
@public
	GHMessageHandler* mMessageHandler;
}

@property(nonatomic, readwrite) GHMessageHandler* mMessageHandler;

@end
