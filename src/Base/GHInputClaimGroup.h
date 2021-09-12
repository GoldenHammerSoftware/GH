// Copyright Golden Hammer Software
#pragma once

#include "GHInputClaim.h"
#include <map>

// Handles claiming/unclaiming pointers for a specific purpose.
//  Allows for the gui to unclaim everything before updating, 
//  without affecting non-gui claims.
class GHInputClaimGroup : public GHInputClaim
{
public:
	GHInputClaimGroup(GHInputClaim& rawClaim);

	void clearClaims(void);
    bool claimPointer(unsigned int index, void* owner);
    void releasePointer(unsigned int index, void* owner);

private:
	GHInputClaim& mRawClaim;
	std::map<unsigned int, void*> mClaims;
};
