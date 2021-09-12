// Copyright 2010 Golden Hammer Software
#include "GHOALInterruptHandler.h"
#include "GHUtils/GHMessage.h"
#include "GHMessageTypes.h"
#include "GHOALSoundDevice.h"

GHOALInterruptHandler::GHOALInterruptHandler(GHOALSoundDevice& soundDevice)
: mDevice(soundDevice)
{
    
}

void GHOALInterruptHandler::handleMessage(const GHMessage& message)
{
    if (message.getType() == GHMessageTypes::PAUSEINTERRUPT)
    {
        mDevice.handlePause();
    }
    else if (message.getType() == GHMessageTypes::UNPAUSEINTERRUPT)
    {
        mDevice.handleUnpause();
    }
}
