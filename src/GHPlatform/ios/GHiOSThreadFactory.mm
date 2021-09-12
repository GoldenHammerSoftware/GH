// Copyright 2010 Golden Hammer Software
#include "GHiOSThreadFactory.h"
#include "GHiOSMutex.h"
#include "GHiOSThreadSleeper.h"
#include "GHiOSThread.h"
#include "GHPlatform/null/GHNullThreadSignal.h"

GHThread* GHiOSThreadFactory::createThread(GHThread::Priority pri) const
{
    float priFloat = 0.5f;
    if (pri == GHThread::TP_HIGH) {
        priFloat = 1.0;
    }
    else if (pri == GHThread::TP_LOW) {
        priFloat = 0.2f;
    }
    
    return new GHiOSThread(priFloat);
}

GHMutex* GHiOSThreadFactory::createMutex(void) const
{
    return new GHiOSMutex();
}

GHThreadSleeper* GHiOSThreadFactory::createSleeper(void) const
{
    return new GHiOSThreadSleeper();
}

GHThreadSignal* GHiOSThreadFactory::createSignal(void) const
{
    return new GHNullThreadSignal();
}

