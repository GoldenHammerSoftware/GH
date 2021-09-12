// Copyright Golden Hammer Software
#include "GHAppRunner.h"
#include "GHApp.h"
#include "GHMessageTypes.h"
#include "GHUtils/GHMessage.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHInputState.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHPlatform/GHThreadFactory.h"
#include "GHPlatform/GHThreadSleeper.h"
#include "GHUtils/GHTransition.h"
#include "GHPlatform/GHMutex.h"
#include <assert.h>

GHAppRunner::GHAppRunner(GHApp* app, 
                         GHMessageHandler& appEventHandler, GHMessageQueue& messageQueue,
                         GHInputState& gameInput, GHInputState& volatileInput,
                         GHMutex& inputMutex, const GHThreadFactory& threadFactory,
                         GHTransition* updatingTransition)
: mApp(app) 
, mAppEventHandler(appEventHandler)
, mAppMessageQueue(messageQueue)
, mStopCalled(false)
, mIsActive(true)
, mGameInputState(gameInput)
, mVolatileInputState(volatileInput)
, mInputMutex(inputMutex)
, mMutex(threadFactory.createMutex())
, mThreadSleeper(threadFactory.createSleeper())
, mUpdatingTransition(updatingTransition)
{
}

GHAppRunner::~GHAppRunner(void)
{
    delete mThreadSleeper;
    delete mMutex;
}

void GHAppRunner::run(void) 
{
    while(true)
    {
        if (mStopCalled)
        {
            return;
        }
        runFrame();
    }
}

void GHAppRunner::runFrame(void)
{
    assert(mApp);
    
    mMutex->acquire();

    if (mUpdatingTransition) mUpdatingTransition->activate();
    mAppMessageQueue.sendMessages(mAppEventHandler);
    
    if (!mIsActive)
    {
        if (mUpdatingTransition) mUpdatingTransition->deactivate();
        mMutex->release();
        // if we don't have focus, don't hog the processor.
        mThreadSleeper->sleep(100);
        return;
    }
    
    mInputMutex.acquire();
    mGameInputState.clearInputEvents();
    mGameInputState.copyValues(mVolatileInputState);
    mVolatileInputState.clearInputEvents();
    mInputMutex.release();
    
    mApp->runFrame();
    if (mUpdatingTransition) mUpdatingTransition->deactivate();
    
    mMutex->release();
}

void GHAppRunner::sendAppMessages(void)
{
	mMutex->acquire();
	mAppMessageQueue.sendMessages(mAppEventHandler);
	mMutex->release();
}

void GHAppRunner::handleImmediateAppMessage(const GHMessage& message)
{
	mMutex->acquire();
	mAppEventHandler.handleMessage(message);
	mMutex->release();
}

void GHAppRunner::prepareForShutdown(void)
{
    if (mStopCalled) return;
    mMutex->acquire();
    mStopCalled = true;
    mMutex->release();
    // give the main thread a chance to exit.
    mThreadSleeper->sleep(500);
}

void GHAppRunner::handleInterrupt(bool active)
{
    if (mIsActive == active)
    {
        return;
    }

    // We used to acquire mMutex here.
    // This is not necessary because mAppMessageQueue has its own mutex.
    // Getting the main mutex here caused massive problems on iOS 12.
    // If there are any problems here then make sure mAppMessageQueue was created with a thread factory.
    // mIsActive was also changed to be an atomic<bool> to avoid issues.
    
    mIsActive = active;
    GHIdentifier messageType = active ? GHMessageTypes::UNPAUSEINTERRUPT : GHMessageTypes::PAUSEINTERRUPT;
    GHMessage interruptMessage(messageType, 0);
    mAppMessageQueue.handleMessage(interruptMessage);
}

