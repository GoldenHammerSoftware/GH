// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include <jni.h>
#include "GHString/GHString.h"
class GHJNIMgr;

class GHAndroidURLOpenerTransition : public GHTransition
{
public:
	GHAndroidURLOpenerTransition(GHJNIMgr& jniMgr, jobject jObject, const char* url);

    virtual void activate(void);
	virtual void deactivate(void) {}

private:
	GHJNIMgr& mJNIMgr;
	jobject mJavaObj;
	jmethodID mOpenURLMethod;
    GHString mURLString;
};