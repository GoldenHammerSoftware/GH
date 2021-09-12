// Copyright 2010 Golden Hammer Software
#import "GHCocoaAppDelegate.h"
#include "GHDebugMessage.h"
#include "GHAppRunner.h"
#include "GHCocoaThread.h"
#include "GHApp.h"
#include "GHInputState.h"
#include "GHMessageQueue.h"
#include "GHMessageTypes.h"
#include "GHMessage.h"
#include "GHResourceCache.h"
#include "GHCacheModelLoader.h"
#include "GHModelLoader.h"
#include "GHMD3ModelLoader.h"
#include "GHFrameAnimSetLoader.h"

#include "GHCocoaRenderServices.h"
#include "GHGameServices.h"

#include "GHDeletionHandle.h"

@implementation GHCocoaAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    mSystemServices = new GHCocoaSystemServices(mView);
    GHDebugMessage::outputString("Launched GHCocoaAppDelegate.");

    mVolatileInputState = new GHInputState(10,1,0,1,2,0,0);
    
    [mWindow setAcceptsMouseMovedEvents:YES];
	[mView setAcceptsTouchEvents:YES];
    [mView initVars];
    [mView setInputState:mVolatileInputState];
    [mView setInputMutex:&mInputMutex];
    
    mUIEventMgr = new GHEventMgr;
    mAppMessageQueue = new GHMessageQueue(&mSystemServices->mPlatformServices->getThreadFactory());
    mUIMessageQueue = new GHMessageQueue(&mSystemServices->mPlatformServices->getThreadFactory());

    mRenderServices = new GHCocoaRenderServices(*mSystemServices, mRenderMutex, *mView);
    mGameServices = new GHGameServices(*mSystemServices, *mRenderServices, *mAppMessageQueue);
    
    mApp = [self createApp];
    mAppRunner = new GHAppRunner(mApp, mSystemServices->mEventMgr, *mAppMessageQueue, 
                                 *mSystemServices->mInputState, *mVolatileInputState, mInputMutex,
                                 mSystemServices->mPlatformServices->getThreadFactory(), 0);

	mRunThread = new GHCocoaThread;
	mRunThread->runThread(*mAppRunner);
	
	[NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(mainThreadUpdate) userInfo:nil repeats:YES];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    GHDebugMessage::outputString("applicationShouldTerminate");
	if (mAppRunner) mAppRunner->prepareForShutdown();

    delete mApp;
    delete mAppRunner; 
    delete mRunThread;
    delete mUIMessageQueue;
    delete mUIEventMgr;
    delete mVolatileInputState;
    delete mRenderServices;
    delete mSystemServices;
	return NSTerminateNow;
}

- (void)windowWillClose:(NSNotification *)notification
{
    GHDebugMessage::outputString("windowWillClose");
    mAppRunner->prepareForShutdown();
}

- (GHApp*) createApp
{
    // create the actual app in subclasses
    return 0;
}

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
	return YES;
}

- (void)mainThreadUpdate
{
	if (mApp) {
        mUIMessageQueue->sendMessages(*mUIEventMgr);
        mUIControllerMgr.update();
	}
}

- (void)windowDidResize:(NSNotification *)notification
{
    if (!mAppMessageQueue) return;
    GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE, 0);
    mAppMessageQueue->handleMessage(resizeMessage);
}

- (void) handleFocusChange:(bool)hasFocus
{
    if (mAppRunner) mAppRunner->handleInterrupt(hasFocus);
    [mView clearInput];
}

- (void)applicationWillBecomeActive:(NSNotification *) notification
{
	[self handleFocusChange:true];
}

- (void)applicationWillResignActive:(NSNotification *) notification
{
	[self handleFocusChange:false];
}

- (void)windowDidDeminiaturize:(NSNotification *) notification
{
	[self handleFocusChange:true];
}

- (void)windowWillMiniaturize:(NSNotification *) notification
{
	[self handleFocusChange:false];
}

@end
