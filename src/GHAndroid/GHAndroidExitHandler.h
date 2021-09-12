// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <jni.h>

class GHEventMgr;
class GHJNIMgr;

class GHAndroidExitHandler : public GHTransition
{
public:
	GHAndroidExitHandler(GHJNIMgr& jniMgr, jobject engineInterface);
    virtual ~GHAndroidExitHandler(void);
    
	virtual void activate(void);
	virtual void deactivate(void) {}

private:
	GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
    jmethodID mExitMethodID;
};
