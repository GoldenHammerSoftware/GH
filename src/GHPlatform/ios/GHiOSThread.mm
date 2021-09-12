// Copyright 2010 Golden Hammer Software
#include "GHiOSThread.h"
#include "GHRunnable.h"
#include <pthread.h>
#include "GHDebugMessage.h"

#define GHOBJC_THREADS

#ifdef GHOBJC_THREADS

@interface ObjectiveCThread : NSObject {
	GHRunnable* mRunnable;
}
- (void)platformRun:(GHRunnable*)runnable priority:(float)pri;
- (void)platformParallel;
@end

@implementation ObjectiveCThread

- (void)platformRun:(GHRunnable*)runnable priority:(float)pri
{
	mRunnable = runnable;
//	[NSThread detachNewThreadSelector:@selector(platformParallel) toTarget:self withObject:nil];
    
    NSThread* myThread = [[NSThread alloc] initWithTarget:self
                                                 selector:@selector(platformParallel)
                                                   object:nil];
    [myThread setThreadPriority:pri];
    [myThread start];
}

- (void)platformParallel {
    @autoreleasepool {
//        GHRunnable* runnable = globalrunnable;
        mRunnable->run();
    }
}

@end

GHiOSThread::GHiOSThread(float pri)
: mPri(pri)
{
}

void GHiOSThread::runThread(GHRunnable& runnable)
{
    // potential leak of ObjectiveCThread
    [[ObjectiveCThread alloc] platformRun:&runnable priority:mPri];
}

#else

GHiOSThread::GHiOSThread(float pri)
: mPri(pri)
{
}

void* threadLaunch(void* arg)
{
    GHRunnable* runnable = (GHRunnable*)arg;
    runnable->run();
    return 0;
}

void GHiOSThread::runThread(GHRunnable& runnable)
{
    pthread_t myPThread;
    if (pthread_create(&myPThread, NULL, threadLaunch, (void*)&runnable))
    {
        GHDebugMessage::outputString("error creating pthread");
    }
}

#endif
