// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGameCenterLocalPlayer.h"

class GHCocoaGameCenterLocalPlayer : public GHGameCenterLocalPlayer
{
public:
    GHCocoaGameCenterLocalPlayer(GHPropertyContainer& propertyContainer);
    virtual void authenticate(void);
};
