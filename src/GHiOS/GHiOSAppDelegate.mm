// Copyright 2010 Golden Hammer Software
#import "GHiOSAppDelegate.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHiOSSystemServices.h"
#include "GHInputState.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHGameServices.h"
#include "GHAppRunner.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThread.h"
#include "GHPlatform/GHMutex.h"
#include "GHApp.h"
#include "GHUtils/GHEventMgr.h"
#include "GHScreenInfo.h"
#include "GHUtils/GHUtilsMessageTypes.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHiOSUIThreadSystemServices.h"
#include "GHRunnableMessageHandler.h"
#include "GHiOSIdentifiers.h"
#include <GameController/GameController.h>
#include "GHiOSAutoreleaseAppRunner.h"

#ifdef GHMETAL
#include "GHMetalRenderServices.h"
#else
#include "GHiOSRenderServices.h"
#endif

// do not check in active.
#ifndef GHRETAIL
//#define USE_NULL_IAP
#endif
#ifdef USE_NULL_IAP
#include "GHNullIAPStoreFactory.h"
#else
#include "GHiOSIAPStoreFactory.h"
#endif

#define kAccelerometerFrequency 60.0

@implementation GHiOSAppDelegate

- (GHApp*) createApp
{
    GHDebugMessage::outputString("createApp not specified");
    // create the actual app in subclasses
    return 0;
}

- (void)applicationDidFinishLaunching:(UIApplication *)application 
{
	[[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0 / kAccelerometerFrequency)];
	[[UIAccelerometer sharedAccelerometer] setDelegate:self];
    
	application.idleTimerDisabled = YES;
    
    mSystemServices = new GHiOSSystemServices(mView->mBehavior);
    GHDebugMessage::outputString("Launched GHiOSAppDelegate.");
    mInputMutex = mSystemServices->getPlatformServices().getThreadFactory().createMutex();
    
    mVolatileInputState = new GHInputState(10,1,1,1,2,0,0);
    
    //[mWindow setAcceptsMouseMovedEvents:YES];
	//[mView setAcceptsTouchEvents:YES];
    mView->mBehavior.setInputState(mVolatileInputState);
    mView->mBehavior.setInputMutex(mInputMutex);
    
    mUIEventMgr = new GHEventMgr;
    
    GHRunnableMessageHandler* runnableHandler = new GHRunnableMessageHandler;
    mUIEventMgr->registerListener(GHUtilsMessageTypes::EXECRUNNABLE, *runnableHandler);
    
    // todo: not leak runnableHandler
    
    mAppMessageQueue = new GHMessageQueue(&mSystemServices->getPlatformServices().getThreadFactory());
    mUIMessageQueue = new GHMessageQueue(&mSystemServices->getPlatformServices().getThreadFactory());
    
    GHiOSUIThreadSystemServices* uiThreadServices = new GHiOSUIThreadSystemServices(mView, mView->mBehavior, *mUIMessageQueue, *mUIEventMgr);
    mSystemServices->addSubService(uiThreadServices);
    
#ifdef USE_NULL_IAP
    mIAPStoreFactory = new GHNullIAPStoreFactory(true);
#else
    mIAPStoreFactory = new GHiOSIAPStoreFactory(mSystemServices->getEventMgr(),
                                                *mAppMessageQueue);
#endif
    
    mRenderMutex = mSystemServices->getPlatformServices().getThreadFactory().createMutex();
    bool allowMSAA = [self usesMSAA];
#ifdef GHMETAL
    mRenderServices = new GHMetalRenderServices(*mSystemServices, *mView);
#else
    mRenderServices = new GHiOSRenderServices(*mSystemServices, *mRenderMutex, *mView, allowMSAA, mSystemServices->getPlatformServices().getIdFactory());
#endif
    mGameServices = new GHGameServices(*mSystemServices, *mRenderServices, *mAppMessageQueue);
    
    mApp = [self createApp];
    
    mAppRunner = new GHiOSAutoreleaseAppRunner(mApp, mSystemServices->getEventMgr(), *mAppMessageQueue,
                                 mSystemServices->getInputState(), *mVolatileInputState, *mInputMutex,
                                 mSystemServices->getPlatformServices().getThreadFactory(), 0);
    
    mRunThread = mSystemServices->getPlatformServices().getThreadFactory().createThread(GHThread::TP_HIGH);
	mRunThread->runThread(*mAppRunner);
    
	[NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(mainThreadUpdate) userInfo:nil repeats:YES];
    
    [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    [[NSNotificationCenter defaultCenter] addObserver: self selector: @selector(deviceOrientationDidChange:) name: UIDeviceOrientationDidChangeNotification object: nil];
     
    if (@available(iOS 6.0, *))
    {
        [[NSNotificationCenter defaultCenter] addObserver:mView selector:@selector(controllerStateChanged) name:GCControllerDidConnectNotification object:nil];
        [[NSNotificationCenter defaultCenter] addObserver:mView selector:@selector(controllerStateChanged) name:GCControllerDidDisconnectNotification object:nil];
        [GCController startWirelessControllerDiscoveryWithCompletionHandler:^(){[mView controllerStateChanged];}];
    }
    if (@available(iOS 14.0, *))
    {
        for (GCMouse* mouse in GCMouse.mice)
        {
            int brkpt = 1;
            brkpt++;
        }
        if (GCKeyboard.coalescedKeyboard)
        {
            int brkpt = 1;
            brkpt++;
        }
    }
}

- (void) applicationWillEnterForeground:(UIApplication *)application
{
    GHMessage message(GHiOSIdentifiers::APPDIDENTERFOREGROUND);
    mUIMessageQueue->handleMessage(message);
    
    // the device may have rotated while asleep.
    // deviceOrientationDidChange will check to see if it changed from what we think it is.
    if (mAppRunner)
    {
        [self deviceOrientationDidChange:nullptr];
    }
}

- (void)mainThreadUpdate
{
	if (mApp) {
        mUIMessageQueue->sendMessages(*mUIEventMgr);
        mUIControllerMgr.update();
	}
}

- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration 
{
    GHPoint3 accel(acceleration.x, acceleration.y, acceleration.z);
    
    //Goal: alter the acceleration values such that:
    //  -"down" wrt the current orientation is (0, -1, 0)
    //  -"left" wrt the current orientation is (-1, 0, 0)
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
    if (orientation == UIInterfaceOrientationLandscapeLeft) {
        //this is the orientation with the round home button on the right
        //raw down: -1, 0, 0
        
        accel[1] = acceleration.x;
        accel[0] = -acceleration.y;
    }
    else if (orientation == UIInterfaceOrientationLandscapeRight) {
        //this is the orientation with the round home button on the left
        //raw down: 1, 0, 0
        
        accel[1] = -acceleration.x;
        accel[0] = acceleration.y;
    }
    else if (orientation == UIInterfaceOrientationPortrait) {
        //raw down: 0, -1, 0
        //do nothing
    }
    else if (orientation == UIInterfaceOrientationPortraitUpsideDown) {
        //raw down: 0, 1, 0
        accel[1] *= -1.f;
        accel[2] *= -1.f;
    }
    
    //screen facing up: 0, 0, -1
    //screen facing down: 0, 0, 1
    
    
    mInputMutex->acquire();
    mVolatileInputState->handleAccelerometer(0, accel);
    mInputMutex->release();
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application 
{
	GHDebugMessage::outputString("LOW MEMORY!!!\n");
}

- (void) handleFocusChange:(bool)hasFocus
{
    if (!mAppRunner) return;
    mAppRunner->handleInterrupt(hasFocus);
    mView->mBehavior.clearInput();
}

- (void)applicationWillTerminate:(UIApplication *)application 
{
	GHDebugMessage::outputString("*** APP FORCED TO TERMINATE ***\n");
	if (mAppRunner) mAppRunner->prepareForShutdown();
}

- (void)applicationWillResignActive:(UIApplication *)application 
{
	[self handleFocusChange:false];
}

- (void)applicationDidBecomeActive:(UIApplication *)application 
{
	[self handleFocusChange:true];
}

- (void)dealloc 
{
    delete mApp;
    delete mAppRunner; 
    delete mRunThread;
    delete mIAPStoreFactory;
    delete mAppMessageQueue;
    delete mUIMessageQueue;
    delete mUIEventMgr;
    delete mVolatileInputState;
    delete mRenderServices;
    delete mSystemServices;
    delete mRenderMutex;
    delete mInputMutex;
}

- (bool)usesMSAA
{
    return true;
}

- (void)deviceOrientationDidChange:(NSNotification *)notification
{
    if (!mRenderServices || !mAppMessageQueue) return;
    
    if(@available(ios 14.0, *))
    {
        if([NSProcessInfo processInfo].iOSAppOnMac)
        {
            // mac doesn't really do orientations.
            return;
        }
    }
    
    UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];

    bool rotateScreen = false;
    GHPoint2i screenSize = mRenderServices->getScreenInfo().getSize();
    if (UIInterfaceOrientationIsPortrait(orientation)) {
        if (screenSize[1] < screenSize[0]) {
            rotateScreen = true;
        }
    }
    else if (UIInterfaceOrientationIsLandscape(orientation)) {
        if (screenSize[0] < screenSize[1]) {
            rotateScreen = true;
        }
    }
    if (rotateScreen)
    {
        int dummy = screenSize[0];
        screenSize[0] = screenSize[1];
        screenSize[1] = dummy;
        mRenderServices->getScreenInfo().setSize(screenSize);
        
        GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
        mAppMessageQueue->handleMessage(resizeMessage);
    }
}

@end
