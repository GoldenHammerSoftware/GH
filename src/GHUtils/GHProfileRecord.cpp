// Copyright Golden Hammer Software
#include "GHProfileRecord.h"
#include "GHPlatform/GHDebugMessage.h"
#include <cstdio>

GHProfileRecord::GHProfileRecord(const char* name, GHString::CharHandlingType handleType)
: mName(name, handleType)
, mStartTime(0)
, mElapsedTime(0)
, mNumCalls(0)
, mRecurseCount(0)
{
}

GHProfileRecord::~GHProfileRecord(void)
{
}

void GHProfileRecord::startTiming(float currTime)
{
    mRecurseCount++;
    if (mRecurseCount == 1)
    {
        mStartTime = currTime;
    }
}

void GHProfileRecord::endTiming(float currTime)
{
    mRecurseCount--;
    if (mRecurseCount == 0)
    {
        mElapsedTime += (currTime - mStartTime);
        mStartTime = 0;
    }
	mNumCalls++;
}

void GHProfileRecord::outputInfo(float totTime)
{
	if (!mNumCalls) return;
	const int BUF_SZ = 512;
	char buff[BUF_SZ];
	if (!totTime) {
		::snprintf(buff, BUF_SZ, "%.2fs (%.2f ms/call) (%d) - %s",
				  mElapsedTime, mElapsedTime*1000/mNumCalls, mNumCalls,
				  mName.getChars());
	}
	else {
		::snprintf(buff, BUF_SZ, "%.2fs (%.2f%%) (%.1f ms/call) (%d) - %s",
				  mElapsedTime, mElapsedTime/totTime*100, 
				  mElapsedTime*1000/mNumCalls, mNumCalls,
				  mName.getChars());
	}
	GHDebugMessage::outputString(buff);
}
