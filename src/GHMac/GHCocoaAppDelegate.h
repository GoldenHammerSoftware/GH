// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHCocoaView.h"
#include "GHCocoaOutputPipe.h"
#include "GHCocoaMutex.h"
#include "GHCocoaSystemServices.h"
#include "GHControllerMgr.h"

class GHApp;
class GHAppRunner;
class GHCocoaThread;
class GHInputState;
class GHMessageQueue;
class GHRenderServices;
class GHGameServices;

@interface GHCocoaAppDelegate : NSObject <NSApplicationDelegate> 
{
@protected
    NSWindow* mWindow;
	GHCocoaView* mView;
    GHApp* mApp;
    GHAppRunner* mAppRunner;
    GHCocoaThread* mRunThread;
    GHMessageQueue* mAppMessageQueue;
    GHMessageQueue* mUIMessageQueue;
    GHEventMgr* mUIEventMgr;
    GHCocoaMutex mRenderMutex;
    GHControllerMgr mUIControllerMgr;
    
    // we pass one input state to the game
    //  and keep another around for modifying mid-frame.
    GHInputState* mGameInputState;
    GHCocoaMutex mInputMutex;

    GHRenderServices* mRenderServices;
    GHCocoaSystemServices* mSystemServices;
    GHGameServices* mGameServices;

@private
    GHInputState* mVolatileInputState;
}
- (GHApp*)createApp;

@end

