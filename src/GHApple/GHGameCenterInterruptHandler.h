// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHGameCenterAuthenticator;

class GHGameCenterInterruptHandler : public GHMessageHandler
{
public:
    GHGameCenterInterruptHandler(GHGameCenterAuthenticator& authenticator);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHGameCenterAuthenticator& mAuthenticator;
};
