// Copyright 2010 Golden Hammer Software
#include "GHiOSAdHandler.h"
#import <UIKit/UIKit.h>
#include "GHUtils/GHEventMgr.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHiOSAdWrapper.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTYpes.h"
#include "GHScreenInfo.h"

GHiOSAdHandler::GHiOSAdHandler(const GHTimeVal& timeVal,
                               GHPropertyContainer& props,
                               const GHIdentifier& displayingAdsProperty,
                               GHEventMgr& gameThreadEventMgr,
                               const GHScreenInfo& screenInfo)
: mAdStack(props, displayingAdsProperty)
, mTimeVal(timeVal)
, mIsPortrait(false)
, mInitialized(false)
, mGameThreadEventMgr(gameThreadEventMgr)
, mMessageListener(*this)
, mPortraitDirty(false)
, mScreenInfo(screenInfo)
{
    updateIsPortrait();
}

GHiOSAdHandler::~GHiOSAdHandler(void)
{
	
}

void GHiOSAdHandler::update(void)
{
    if (mPortraitDirty) {
        updateIsPortrait();
        mPortraitDirty = false;
    }
	mAdStack.update(mTimeVal);
}

void GHiOSAdHandler::onActivate(void)
{
    mGameThreadEventMgr.registerListener(GHMessageTypes::WINDOWRESIZE, mMessageListener);
    if (!mInitialized) 
    {
        mAdStack.initialize();
        mInitialized = true;
	}
    mAdStack.activate();    
}

void GHiOSAdHandler::onDeactivate(void)
{
    mGameThreadEventMgr.unregisterListener(GHMessageTypes::WINDOWRESIZE, mMessageListener);
    mAdStack.deactivate();
}

void GHiOSAdHandler::addWrapper(GHiOSAdWrapper* wrapper)
{
    mAdStack.addWrapper(wrapper);
    wrapper->setOrientation(mIsPortrait);
}

void GHiOSAdHandler::updateIsPortrait(void)
{
    mIsPortrait = false;
    
    if (mScreenInfo.getSize()[0] < mScreenInfo.getSize()[1]) {
        mIsPortrait = true;
    }
    /*
    if ([[UIApplication sharedApplication] statusBarOrientation] == UIInterfaceOrientationPortrait ||
        [[UIApplication sharedApplication] statusBarOrientation] == UIInterfaceOrientationPortraitUpsideDown)
    {
        mIsPortrait = true;
    }
    */
    mAdStack.updateIsPortrait(mIsPortrait);
}

void GHiOSAdHandler::handleWindowSizeChanged(void)
{
    // we set a flag for processing the next time update comes around
    // otherwise we are changing iads on the wrong thread.
    mPortraitDirty = true;
}

void GHiOSAdHandler::MessageListener::handleMessage(const GHMessage& message)
{
    mParent.handleWindowSizeChanged();
}

