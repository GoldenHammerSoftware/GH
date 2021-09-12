// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"
#include "GHUtils/GHProperty.h"
#include <vector>

class GHPropertyContainer;

// A transition to toggle a property through a list of values on activate.
// Every time we activate we cycle to the next val in our list.
// Can also be used as a property setter by having only one val in the list.
class GHPropertyToggleTransition : public GHTransition
{
public:
    GHPropertyToggleTransition(GHPropertyContainer& props, const GHIdentifier& propId);
    
    virtual void activate(void);
	virtual void deactivate(void);

    void addValue(const GHProperty& val) { mVals.push_back(val); }
    void addDeactivateValue(const GHProperty& val) { mDeactivateVals.push_back(val); }
    
private:
    void toggleProperty(std::vector<GHProperty>& valList);
    
private:
    GHPropertyContainer& mProps;
    GHIdentifier mPropId;
    std::vector<GHProperty> mVals;
    std::vector<GHProperty> mDeactivateVals;
};
