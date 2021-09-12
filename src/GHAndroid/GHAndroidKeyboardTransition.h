// Copyright 2020 Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

#include <jni.h>
class GHJNIMgr;

// A transition that shows a keyboard on activate and hides it on deactivate
class GHAndroidKeyboardTransition : public GHTransition
{
public:
	GHAndroidKeyboardTransition(GHJNIMgr& jniMgr, jobject engineInterface);
	~GHAndroidKeyboardTransition(void);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHJNIMgr& mJNIMgr;
	jobject mEngineInterface;
	jmethodID mShowMethod;
	jmethodID mHideMethod;
};

