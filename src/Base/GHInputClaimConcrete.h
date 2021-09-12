// Copyright Golden Hammer Software
#pragma once

#include "GHInputClaim.h"
#include <vector>

// concrete implementation of no frills pointer claim.
class GHInputClaimConcrete : public GHInputClaim
{
public:
	GHInputClaimConcrete(void);

    virtual bool claimPointer(unsigned int index, void* owner);
    virtual void releasePointer(unsigned int index, void* owner);

private:
	std::vector<void*> mPointers;
};
