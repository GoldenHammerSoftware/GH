// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterLocalPlayer.h"

class GHiOSGameCenterLocalPlayer : public GHGameCenterLocalPlayer
{
public:
    GHiOSGameCenterLocalPlayer(GHPropertyContainer& propertyContainer, UIViewController* viewController);
    
    virtual void authenticate(void);
    virtual void showAuthenticationViewController(void);
    
private:
    UIViewController* mViewController;
};
