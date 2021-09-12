// Copyright Golden Hammer Software
#pragma once

#include "GHAdTransition.h"

class GHAdHandlingDelegate;

// An ad transition that just exists as soon as it is called.
class GHInstantAbortAd : public GHAdTransition
{
public:
    GHInstantAbortAd(GHAdHandlingDelegate& delegate);

    // call ad delegate dismissed.
  	virtual void activate(void);
	virtual void deactivate(void) {}

    virtual bool hasAd(void) const { return true; }

private:
    GHAdHandlingDelegate& mDelegate;
};
