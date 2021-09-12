// Copyright Golden Hammer Software
#include "GHUtils/GHPropertyStacker.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyStacker::GHPropertyStacker(GHPropertyContainer& props, const GHIdentifier& propId, const GHProperty& val)
: mProps(props)
, mPropId(propId)
{
    mPreviousVal = mProps.getProperty(mPropId);
    mProps.setProperty(mPropId, val);
}

GHPropertyStacker::~GHPropertyStacker(void)
{
    mProps.setProperty(mPropId, mPreviousVal);
}
