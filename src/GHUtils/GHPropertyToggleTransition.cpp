// Copyright Golden Hammer Software
#include "GHPropertyToggleTransition.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyToggleTransition::GHPropertyToggleTransition(GHPropertyContainer& props, const GHIdentifier& propId)
: mProps(props)
, mPropId(propId)
{
}

void GHPropertyToggleTransition::activate(void)
{
    toggleProperty(mVals);
}

void GHPropertyToggleTransition::deactivate(void)
{
    toggleProperty(mDeactivateVals);
}

void GHPropertyToggleTransition::toggleProperty(std::vector<GHProperty>& valList)
{
	if (!valList.size()) return;
    const GHProperty& currVal = mProps.getProperty(mPropId);
    for (size_t i = 0; i < valList.size(); ++i) {
        if (currVal == valList[i]) {
            size_t setIdx = i+1;
            if (setIdx >= valList.size()) setIdx = 0;
            mProps.setProperty(mPropId, valList[setIdx]);
            return;
        }
    }
    if (valList.size()) {
        mProps.setProperty(mPropId, valList[0]);
    }
}
