// Copyright 2020 Golden Hammer Software
#include "GHAndroidKeyboardTransition.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include "GHPlatform/GHDebugMessage.h"

GHAndroidKeyboardTransition::GHAndroidKeyboardTransition(GHJNIMgr& jniMgr, jobject engineInterface)
	: mJNIMgr(jniMgr)
	, mEngineInterface(engineInterface) 
{
	jclass engInterfaceCls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if (engInterfaceCls == 0) {
		GHDebugMessage::outputString("GHAndroidKeyboardTransition can't find class where keyboard methods should exist\n.");
	}

	mShowMethod = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "showKeyboard", "()V");
	mHideMethod = mJNIMgr.getJNIEnv().GetMethodID(engInterfaceCls, "hideKeyboard", "()V");
}

GHAndroidKeyboardTransition::~GHAndroidKeyboardTransition(void)
{
}

void GHAndroidKeyboardTransition::activate(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mShowMethod);
}

void GHAndroidKeyboardTransition::deactivate(void)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mHideMethod);
}
