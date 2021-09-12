// Copyright Golden Hammer Software
#include "GHInstantAbortAd.h"
#include "GHAdHandlingDelegate.h"

GHInstantAbortAd::GHInstantAbortAd(GHAdHandlingDelegate& delegate)
: mDelegate(delegate)
{
}

void GHInstantAbortAd::activate(void)
{
    mDelegate.adDeactivated(this);
}
