// Copyright Golden Hammer Software
#pragma once

class GHTimeVal;

// A little util class to monitor frame changes to avoid doing unecessary work.
class GHNewFrameChecker
{
public:
	GHNewFrameChecker(void) : mLastUpdateTime(0) {}
	
	// if mLastUpdateTime is different than current time, return true
	//  also update mLastUpdateTime when returning true.
	bool checkIsNewFrame(const GHTimeVal& time);
	
protected:
	float mLastUpdateTime;
};
