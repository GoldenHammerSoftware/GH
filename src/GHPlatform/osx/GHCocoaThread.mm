// Copyright 2010 Golden Hammer Software
#include "GHCocoaThread.h"
#include "GHRunnable.h"

@interface ObjectiveCThread : NSObject {
	GHRunnable* mRunnable;
}
- (void)platformRun:(GHRunnable*)runnable;
- (void)platformParallel;
@end

@implementation ObjectiveCThread

- (void)platformRun:(GHRunnable*)runnable {
	mRunnable = runnable;
	[NSThread detachNewThreadSelector:@selector(platformParallel) toTarget:self withObject:nil]; 	
}

- (void)platformParallel {
	@autoreleasepool {
		mRunnable->run();
	}
}

@end

void GHCocoaThread::runThread(GHRunnable& runnable)
{
	[[ObjectiveCThread alloc] platformRun:&runnable];
}
