// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHiOSAdWrapper.h"

class GHHouseAd;
class GHXMLObjFactory;

class GHiOSHouseAdWrapper : public GHiOSAdWrapper
{
public:
    GHiOSHouseAdWrapper(GHHouseAd* houseAd);
    ~GHiOSHouseAdWrapper(void);
    
    virtual void initialize(GHAdHandlingDelegate* adHandlingDelegate, int adID);
    virtual void setOrientation(bool isPortrait);
    virtual void update(const GHTimeVal& timeVal);
    virtual void setActive(bool active);
    
private:
    GHHouseAd* mHouseAd;
    GHAdHandlingDelegate* mAdHandlingDelegate;
    int mAdID;
    bool mIsActive{false};
    bool mLaunchTimeWaitHappened{false};
};
