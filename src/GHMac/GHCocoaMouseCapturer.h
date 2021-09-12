// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHMouseCapturer.h"
#import "GHCocoaView.h"

class GHCocoaMouseCapturer : public GHMouseCapturer
{
public:
    GHCocoaMouseCapturer(GHCocoaView* view);
    
    virtual void captureMouse(void);
    virtual void releaseMouse(void);
    
    virtual void setMousePos(float x, float y);
    
private:
    void handleMouseVisCountChange(void);
    
private:
    GHCocoaView* mView;
    int mMouseVisCount;
};
