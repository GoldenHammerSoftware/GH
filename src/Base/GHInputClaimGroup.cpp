// Copyright Golden Hammer Software
#include "GHInputClaimGroup.h"
#include "GHInputState.h"

GHInputClaimGroup::GHInputClaimGroup(GHInputClaim& rawClaim)
: mRawClaim(rawClaim)
{
}

void GHInputClaimGroup::clearClaims(void)
{
	std::map<unsigned int, void*>::iterator iter;
	for (iter = mClaims.begin(); iter != mClaims.end(); ++iter) {
		mRawClaim.releasePointer(iter->first, iter->second);
	}
	mClaims.clear();
}

bool GHInputClaimGroup::claimPointer(unsigned int index, void* owner)
{
	if (mRawClaim.claimPointer(index, owner)) {
		mClaims[index] = owner;
		return true;
	}
	return false;
}

void GHInputClaimGroup::releasePointer(unsigned int index, void* owner)
{
	mRawClaim.releasePointer(index,owner);
	std::map<unsigned int, void*>::iterator iter;
	for (iter = mClaims.begin(); iter != mClaims.end(); ++iter) {
		if (iter->first == index) {
			if (iter->second == owner) {
				mClaims.erase(iter);
				return;
			}
		}
	}
}
