// Copyright Golden Hammer Software
#pragma once

#include <map>
#include "GHString/GHString.h"

class GHProfileRecord;
class GHTimeCalculator;
class GHMutex;

/// A class to monitor how long things take.
class GHProfiler
{
public:
	// The profiler is a singleton because it should be easy to 
	//  temporarily drop in to any code for debugging.
	static GHProfiler* getProfiler(void);
	static void initProfiler(const GHTimeCalculator& timeMgr, GHMutex* mutex);
	
public:
	GHProfiler(const GHTimeCalculator& timeMgr, GHMutex* mutex);
	~GHProfiler(void);
	
	/// Start monitoring timing calls.
	void beginProfiling(void);
	/// Stop monitoring timing calls.
	void endProfiling(void);
	/// Dump the results to the debug stream.
	void outputResults(void);
	/// Reset the results.
	void clearResults(void);
	
	/// Start a timing block with a given name.
	void startProfile(const char* name, GHString::CharHandlingType handleType=GHString::CHT_REFERENCE);
	/// End the block of timing and record elapsed time.
	void endProfile(const char* name);
	// set which profile is used for the 100% number
	void setTotalProfile(const char* name) { mTotalProfileName = name; }
	
	float getTime(void);
	
protected:
	typedef std::map<GHString, GHProfileRecord*> ProfileRecordMap;
	ProfileRecordMap mRecords;
	const char* mTotalProfileName;
	bool mIsProfiling;
	const GHTimeCalculator& mTimeCalculator;
    GHMutex* mMutex;
	
	static GHProfiler* sProfiler;
};

// A class that starts a profile on create, and stops when it goes out of scope.
class GHProfileObj
{
public:
	GHProfileObj(const char* str, GHString::CharHandlingType handleType=GHString::CHT_REFERENCE);
	~GHProfileObj(void);

private:
	GHString mStr;
};

#ifndef GHRETAIL
    #define GHPROFILEBEGIN(x) if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->startProfile(x); }
    #define GHPROFILEBEGINARGS(x,y) if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->startProfile(x,y); }
    #define GHPROFILEEND(x) if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->endProfile(x); }
    #define GHPROFILESCOPE(x,y) GHProfileObj prof##__FILE__##__LINE__(x,y);
	#define GHPROFILESTART if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->beginProfiling(); }
	#define GHPROFILESTOP if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->endProfiling(); }
	#define GHPROFILEOUTPUT if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->outputResults(); }
	#define GHPROFILECLEAR if (GHProfiler::getProfiler()) { GHProfiler::getProfiler()->clearResults(); }
#else
    #define GHPROFILEBEGIN(x) 
    #define GHPROFILEBEGINARGS(x,y) 
    #define GHPROFILEEND(x) 
    #define GHPROFILESCOPE(x,y)
	#define GHPROFILESTART
	#define GHPROFILESTOP
	#define GHPROFILEOUTPUT
	#define GHPROFILECLEAR
#endif

