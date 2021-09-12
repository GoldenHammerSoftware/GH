// Copyright 2019 Golden Hammer Software
#include "GHCocoaPlatformServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHProfiler.h"

GHCocoaPlatformServices::GHCocoaPlatformServices(void)
{
    GHDebugMessage::init(mOutputPipe);
    GHProfiler::initProfiler(getTimeCalculator(), getThreadFactory().createMutex());
}

