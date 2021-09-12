// Copyright 2010 Golden Hammer Software
#include "GHAVFSoundHandle.h"
#include "GHAVFInterruptionDelegate.h"

namespace {
	struct InterruptDelegateSingleton
	{
		GHAVFInterruptionDelegate* mDelegate;
		InterruptDelegateSingleton(void)
		{
			mDelegate = [GHAVFInterruptionDelegate alloc];
			mDelegate->mMessageHandler = nil;
		}
		~InterruptDelegateSingleton(void)
		{
		}
	};
	InterruptDelegateSingleton sDelegateSingleton;
}

void GHAVFSoundHandle::setMessageHandler(GHMessageHandler* messageHandler)
{
	sDelegateSingleton.mDelegate->mMessageHandler = messageHandler;
}

GHAVFSoundHandle::GHAVFSoundHandle(NSURL* url)
: mAudioPlayer(nil)
{
	NSError* error;
	mAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:url error:&error];
	
	[mAudioPlayer setDelegate:sDelegateSingleton.mDelegate];
	
	if(mAudioPlayer == nil) {
		NSLog(@"%@", [error description]);
	}
}

GHAVFSoundHandle::~GHAVFSoundHandle(void)
{
}
