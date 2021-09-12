// Copyright 2010 Golden Hammer Software
#include "GHGameCenterInterruptHandler.h"
#include "GHGameCenterAuthenticator.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHAppleIdentifiers.h"

GHGameCenterInterruptHandler::GHGameCenterInterruptHandler(GHGameCenterAuthenticator& authenticator)
: mAuthenticator(authenticator)
{
    
}

void GHGameCenterInterruptHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
        mAuthenticator.handleUnpause();
    }
    else if (message.getType() == GHAppleIdentifiers::M_GC_SHOWAUTHVIEW)
    {
        mAuthenticator.showAuthView();
    }
}
