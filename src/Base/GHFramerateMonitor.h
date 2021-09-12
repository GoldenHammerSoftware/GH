// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"

class GHTimeVal;
class GHPropertyContainer;

// a controller to monitor the framerate and fill in a property
class GHFramerateMonitor : public GHController
{
public:
    GHFramerateMonitor(const GHTimeVal& timeVal,
                       GHPropertyContainer& props,
                       const GHIdentifier& fpsProp,
                       const GHIdentifier& msProp);
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
#define HISTORY_LEN 10

    const GHTimeVal& mTimeVal;
    GHPropertyContainer& mProps;
    GHIdentifier mFPSProp;
    GHIdentifier mMSProp;

	int mCurrentHistory;
	float mTimeHistory[HISTORY_LEN];
};
