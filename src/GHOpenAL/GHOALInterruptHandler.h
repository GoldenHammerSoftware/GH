// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHMessageHandler.h"

class GHOALSoundDevice;

class GHOALInterruptHandler : public GHMessageHandler
{
public:
    GHOALInterruptHandler(GHOALSoundDevice& soundDevice);
    
    virtual void handleMessage(const GHMessage& message);
    
private:
    GHOALSoundDevice& mDevice;
};
