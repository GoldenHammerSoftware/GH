// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHTimeCalculator.h"

class GHiOSTimeCalculator : public GHTimeCalculator
{
public:
    GHiOSTimeCalculator(void);
    virtual float calcTime(void) const;
    
private:
    double mStartupTime;
};
