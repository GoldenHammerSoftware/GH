// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;

// A transition to copy a property from one container to another on activate
class GHPropertyCopyTransition : public GHTransition
{
public:
    GHPropertyCopyTransition(const GHPropertyContainer& sourceMap,
                             const GHIdentifier& sourceId,
                             GHPropertyContainer& destMap,
                             const GHIdentifier& destId);
    
    virtual void activate(void);
	virtual void deactivate(void) {}
    
private:
    const GHPropertyContainer& mSourceMap;
    GHIdentifier mSourceId;
    GHPropertyContainer& mDestMap;
    GHIdentifier mDestId;
};
