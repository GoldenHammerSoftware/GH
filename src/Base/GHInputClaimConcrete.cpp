// Copyright Golden Hammer Software
#include "GHInputClaimConcrete.h"
#include <cassert>
#include <stddef.h>

GHInputClaimConcrete::GHInputClaimConcrete(void)
{
}

bool GHInputClaimConcrete::claimPointer(unsigned int index, void* owner)
{
	while (mPointers.size() < index+1)
	{
		mPointers.push_back(0);
	}
    if (owner == mPointers[index]) return true;
    else if (mPointers[index] != 0) return false;
    mPointers[index] = owner;
    return true;
}

void GHInputClaimConcrete::releasePointer(unsigned int index, void* owner)
{
	if (index >= mPointers.size()) return;
    if (owner == mPointers[index])
    {
        mPointers[index] = 0;
    }
}
