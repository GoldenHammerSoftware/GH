// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHString.h"

class GHProfileRecord
{
public:
	GHProfileRecord(const char* name, GHString::CharHandlingType handleType);
	~GHProfileRecord(void);
	
	void startTiming(float currTime);
	void endTiming(float currTime);
	
	void outputInfo(float totTime);
	
	float getElapsedTime(void) const { return mElapsedTime; }
	
protected:
	GHString mName;
	float mStartTime;
	float mElapsedTime;
	int mNumCalls;
    int mRecurseCount;
};
