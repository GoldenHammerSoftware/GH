// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMessageHandler.h"
#include "GHTransition.h"
#import "GHCocoaView.h"

class GHEventMgr;
class GHPropertyContainer;

//A class that manages whether we get touchpad input or whether
// the touchpad controls Mouse 0.
class GHCocoaTouchpadMouseSeparator : public GHTransition
{
public:
    GHCocoaTouchpadMouseSeparator(GHCocoaView* cocoaView);
    
    virtual void activate(void);
    virtual void deactivate(void);
    
private:
    GHCocoaView* mCocoaView;
};
