// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHTimeCalculator.h"
#include <jni.h>

class GHJNIMgr;

class GHAndroidTimeCalculator : public GHTimeCalculator
{
public:
	GHAndroidTimeCalculator(GHJNIMgr& jniMgr, jobject jobj);
	virtual float calcTime(void) const;
    
private:
	GHJNIMgr& mJNIMgr;
    jobject mJObject;
    jmethodID mMethodID;
    
    long mStartTime;
};
