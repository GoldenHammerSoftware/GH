// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTimeCalculator.h"

class GHCocoaTimeCalculator : public GHTimeCalculator
{
public:
    GHCocoaTimeCalculator(void);
    
    virtual float calcTime(void) const;
    
private:
    double mStartupTime;
};
