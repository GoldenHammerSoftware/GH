// Copyright 2010 Golden Hammer Software
#pragma once

#import <UIKit/UIKit.h>
#import "GHiOSView.h"
#include "GHUtils/GHControllerMgr.h"

class GHApp;
class GHThread;
class GHMessageQueue;
class GHMutex;
class GHAppRunner;
class GHInputState;
class GHRenderServices;
class GHSystemServices;
class GHGameServices;
class GHIAPStoreFactory;
class GHEventMgr;

@interface GHiOSAppDelegate : NSObject <UIApplicationDelegate, UIAccelerometerDelegate>
{
@protected
    UIWindow* mWindow;
	GHiOSView* mView;
    GHApp* mApp;
    GHAppRunner* mAppRunner;
    GHThread* mRunThread;
    GHMessageQueue* mAppMessageQueue;
    GHMessageQueue* mUIMessageQueue;
    GHEventMgr* mUIEventMgr;
    GHControllerMgr mUIControllerMgr;
    GHIAPStoreFactory* mIAPStoreFactory;
    GHMutex* mRenderMutex;
    
    // we pass one input state to the game
    //  and keep another around for modifying mid-frame.
    GHInputState* mGameInputState;
    GHMutex* mInputMutex;
    
    GHRenderServices* mRenderServices;
    GHSystemServices* mSystemServices;
    GHGameServices* mGameServices;
    
@private
    GHInputState* mVolatileInputState;
}
- (GHApp*)createApp;
- (bool)usesMSAA;
- (void)deviceOrientationDidChange:(NSNotification *)notification;

@end
