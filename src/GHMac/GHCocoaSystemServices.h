// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSystemServices.h"
#import "GHCocoaView.h"

class GHCocoaSystemServices : public GHSystemServices
{
public:
    GHCocoaSystemServices(GHCocoaView* view);
    
    virtual void initAppShard(GHAppShard& appShard);
    
private:
    GHCocoaView* mCocoaView;
};
