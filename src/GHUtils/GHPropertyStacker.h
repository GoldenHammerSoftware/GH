// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"
#include "GHUtils/GHProperty.h"

class GHPropertyContainer;

// Util class to make a property container act as a stack.
// Sets a property and stores old value on create.
// Puts the property back how it was on destroy.
class GHPropertyStacker
{
public:
    GHPropertyStacker(GHPropertyContainer& props, const GHIdentifier& propId, const GHProperty& val);
    ~GHPropertyStacker(void);
    
private:
    GHPropertyContainer& mProps;
    GHIdentifier mPropId;
    GHProperty mPreviousVal;
};
