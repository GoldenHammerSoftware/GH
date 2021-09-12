// Copyright Golden Hammer Software
#include "GHNewFrameChecker.h"
#include "GHPlatform/GHTimeVal.h"

bool GHNewFrameChecker::checkIsNewFrame(const GHTimeVal& time)
{
	if (time.getTime() != mLastUpdateTime) {
		mLastUpdateTime = time.getTime();
		return true;
	}
	return false;
}

