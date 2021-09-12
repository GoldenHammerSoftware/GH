// Copyright 2010 Golden Hammer Software
#include "GHGameCenterAuthenticator.h"
#include "GHGameCenterLocalPlayer.h"
#include "GHGameCenterInterruptHandler.h"
#include "GHUtils/GHEventMgr.h"
#include "GHMessageTypes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHAppleIdentifiers.h"

GHGameCenterAuthenticator::GHGameCenterAuthenticator(GHEventMgr& eventMgr,
                                                     GHGameCenterLocalPlayer& localPlayer)
: mEventMgr(eventMgr)
, mLocalPlayer(localPlayer)
, mNeedsAuthentication(true)
{
    mInterruptHandler = new GHGameCenterInterruptHandler(*this);
    mEventMgr.registerListener(GHMessageTypes::UNPAUSEINTERRUPT, *mInterruptHandler);
    mEventMgr.registerListener(GHAppleIdentifiers::M_GC_SHOWAUTHVIEW, *mInterruptHandler);
}

GHGameCenterAuthenticator::~GHGameCenterAuthenticator(void)
{
    mEventMgr.unregisterListener(GHMessageTypes::UNPAUSEINTERRUPT, *mInterruptHandler);
    mEventMgr.unregisterListener(GHAppleIdentifiers::M_GC_SHOWAUTHVIEW, *mInterruptHandler);
    delete mInterruptHandler;
}

void GHGameCenterAuthenticator::onActivate(void)
{
    mNeedsAuthentication = true;
}

void GHGameCenterAuthenticator::onDeactivate(void)
{
    
}

void GHGameCenterAuthenticator::update(void)
{
    if (mNeedsAuthentication)
    {
        mLocalPlayer.authenticate();
        mNeedsAuthentication = false;
    }
}

void GHGameCenterAuthenticator::handleUnpause(void)
{
    GHDebugMessage::outputString("GHGameCenterAuthenticator::handleUnpause called");
    mNeedsAuthentication = true;
}

bool GHGameCenterAuthenticator::isAuthenticated(void) const
{
    return mLocalPlayer.isAuthenticated();
}

void GHGameCenterAuthenticator::showAuthView(void)
{
    mLocalPlayer.showAuthenticationViewController();
}
