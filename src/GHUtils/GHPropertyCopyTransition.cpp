// Copyright Golden Hammer Software
#include "GHPropertyCopyTransition.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyCopyTransition::GHPropertyCopyTransition(const GHPropertyContainer& sourceMap,
                                                   const GHIdentifier& sourceId,
                                                   GHPropertyContainer& destMap,
                                                   const GHIdentifier& destId)
: mSourceMap(sourceMap)
, mSourceId(sourceId)
, mDestMap(destMap)
, mDestId(destId)
{
}

void GHPropertyCopyTransition::activate(void)
{
    const GHProperty& sourceProp = mSourceMap.getProperty(mSourceId);
    mDestMap.setProperty(mDestId, sourceProp);
}
