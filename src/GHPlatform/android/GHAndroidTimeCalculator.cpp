// Copyright 2010 Golden Hammer Software
#include "GHAndroidTimeCalculator.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHJNIMgr.h"

GHAndroidTimeCalculator::GHAndroidTimeCalculator(GHJNIMgr& jniMgr, jobject jobj)
: mJNIMgr(jniMgr)
, mJObject(jobj)
, mMethodID(0)
, mStartTime(0)
{
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJObject);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidTimeCalculator can't find class where getCurrTime method should exist\n.");
	}
	mMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "getCurrTime", "()J");
	if(mMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidTimeCalculator can't find getCurrTime method in Java.\n");
	}
    
	mStartTime = mJNIMgr.getJNIEnv().CallLongMethod(mJObject, mMethodID);
}

float GHAndroidTimeCalculator::calcTime(void) const
{
    long currTime = mJNIMgr.getJNIEnv().CallLongMethod(mJObject, mMethodID);
    long currModTime = currTime - mStartTime;
	float fcurrTime = (float)(currModTime) / 1000.0f;
    
    return fcurrTime;
}

