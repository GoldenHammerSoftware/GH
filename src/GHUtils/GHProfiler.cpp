// Copyright Golden Hammer Software
#include "GHProfiler.h"
#include "GHProfileRecord.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "GHPlatform/GHDebugMessage.h"
#include <vector>
#include <algorithm>
#include "GHPlatform/GHMutex.h"

struct ProfileRecordLess
{
	bool operator()(GHProfileRecord* r1, GHProfileRecord* r2) const 
	{ 
		if (r1->getElapsedTime() < r2->getElapsedTime()) return false;
		if (r1->getElapsedTime() > r2->getElapsedTime()) return true;
		return r1 < r2;
	}
};

GHProfiler* GHProfiler::sProfiler = 0;

GHProfiler* GHProfiler::getProfiler(void)
{
	return sProfiler;
}
void GHProfiler::initProfiler(const GHTimeCalculator& timeMgr, GHMutex* mutex)
{
	sProfiler = new GHProfiler(timeMgr, mutex);
}

GHProfiler::GHProfiler(const GHTimeCalculator& timeCalculator, GHMutex* mutex)
: mIsProfiling(false)
, mTimeCalculator(timeCalculator)
, mTotalProfileName(0)
, mMutex(mutex)
{
}

GHProfiler::~GHProfiler(void)
{
	clearResults();
    if (mMutex) {
        mMutex->acquire();
        mMutex->release();
        delete mMutex;
    }
}

void GHProfiler::beginProfiling(void)
{
	mIsProfiling = true;
}

void GHProfiler::endProfiling(void)
{
	mIsProfiling = false;
}

void GHProfiler::startProfile(const char* name, GHString::CharHandlingType handleType)
{
	if (!mIsProfiling) return;
    if (mMutex) mMutex->acquire();
    
	ProfileRecordMap::iterator findIter;
	findIter = mRecords.find(name);
	
	GHProfileRecord* record = 0;
	if (findIter != mRecords.end()) {
		record = findIter->second;
	}
	else {
		record = new GHProfileRecord(name, handleType);
		mRecords[GHString(name, handleType)] = record;
	}
	
	record->startTiming(mTimeCalculator.calcTime());
    if (mMutex) mMutex->release();
}

void GHProfiler::endProfile(const char* name)
{
	if (!mIsProfiling) return;
    if (mMutex) mMutex->acquire();

	ProfileRecordMap::iterator findIter;
	findIter = mRecords.find(name);
	
	if (findIter == mRecords.end()) {
        if (mMutex) mMutex->release();
		return;
	}
	GHProfileRecord* record = findIter->second;
	
	record->endTiming(mTimeCalculator.calcTime());
    if (mMutex) mMutex->release();
}

void GHProfiler::outputResults(void)
{
    if (mMutex) mMutex->acquire();
	GHDebugMessage::outputString("**** PROFILE RESULTS ****");
	
	std::vector<GHProfileRecord*> sortedList;
	
	ProfileRecordMap::const_iterator rIter;
	for (rIter = mRecords.begin(); rIter != mRecords.end(); ++rIter) {
		sortedList.push_back(rIter->second);
	}
	
	float totTime = 0;
	if (mTotalProfileName)
	{
		ProfileRecordMap::iterator findIter;
		findIter = mRecords.find(mTotalProfileName);
		if (findIter != mRecords.end())
		{
			totTime = findIter->second->getElapsedTime();
		}
	}
	
	ProfileRecordLess sorter;
	std::sort(sortedList.begin(), sortedList.end(), sorter);
	std::vector<GHProfileRecord*>::const_iterator sIter;
	for (sIter = sortedList.begin(); sIter != sortedList.end(); ++sIter) {
		(*sIter)->outputInfo(totTime);
	}
    if (mMutex) mMutex->release();
}

void GHProfiler::clearResults(void)
{
    if (mMutex) mMutex->acquire();
	ProfileRecordMap::const_iterator rIter;
	for (rIter = mRecords.begin(); rIter != mRecords.end(); ++rIter) {
		delete rIter->second;
	}
	mRecords.clear();
    if (mMutex) mMutex->release();
}

float GHProfiler::getTime(void)
{
	return mTimeCalculator.calcTime();
}

GHProfileObj::GHProfileObj(const char* str, GHString::CharHandlingType handleType)
: mStr(str, handleType)
{
	GHPROFILEBEGINARGS(str, handleType);
}

GHProfileObj::~GHProfileObj(void)
{
	GHPROFILEEND(mStr.getChars());
}
