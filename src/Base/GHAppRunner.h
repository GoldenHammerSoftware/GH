// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRunnable.h"
#include <atomic>

class GHMutex;
class GHApp;
class GHMessageHandler;
class GHInputState;
class GHMessageQueue;
class GHThreadFactory;
class GHThreadSleeper;
class GHTransition;
class GHMessage;

// A runnable for updating the app in a thread.
class GHAppRunner : public GHRunnable
{
public:
	GHAppRunner(GHApp* app, 
                GHMessageHandler& appEventHandler, GHMessageQueue& messageQueue,
                GHInputState& gameInput, GHInputState& volatileInput,
                GHMutex& inputMutex, const GHThreadFactory& threadFactory,
                GHTransition* updatingTransition);
    ~GHAppRunner(void);
    
    // run forever, until stop called.
	virtual void run(void);
    // run one frame.
    // made virtual so ios can insert an autoreleasepool.
    virtual void runFrame(void);
    
	void prepareForShutdown(void);
    void handleInterrupt(bool hasFocus);

	GHMutex& getMutex(void) { return *mMutex; }
    
	// a way to pump the app message queue outside of the usual tick.
	void sendAppMessages(void);
	// send an app message right now, or as soon as we can acquire the run mutex.
	void handleImmediateAppMessage(const GHMessage& message);

private:
    GHMutex* mMutex;
    GHThreadSleeper* mThreadSleeper;
    GHMessageHandler& mAppEventHandler;
    GHMessageQueue& mAppMessageQueue;
	GHApp* mApp;
    GHInputState& mGameInputState;
    GHInputState& mVolatileInputState;
    GHMutex& mInputMutex;
    bool mStopCalled;
    std::atomic<bool> mIsActive;
    
    // an optional transition that begins and ends around the update.
    GHTransition* mUpdatingTransition;
};
