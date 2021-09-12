// Copyright 2019 Golden Hammer Software
#include "GHiOSPlatformServices.h"
#include "GHPlatform/GHDebugMessage.h"

GHiOSPlatformServices::GHiOSPlatformServices(void)
{
    GHDebugMessage::init(mOutputPipe);
}

