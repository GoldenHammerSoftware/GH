// Copyright 2010 Golden Hammer Software
#include "GHAndroidExitHandler.h"
#include "GHMessageTypes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidExitHandler::GHAndroidExitHandler(GHJNIMgr& jniMgr, jobject engineInterface)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
, mExitMethodID(0)
{
    jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mEngineInterface);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidExitHandler can't find class where exit method should exist\n.");
	}
	mExitMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "exitApp", "()V");
	if(mExitMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidExitHandler can't find exitApp method in Java.\n");
	}
}

GHAndroidExitHandler::~GHAndroidExitHandler(void)
{
}

void GHAndroidExitHandler::activate(void)
{
    if (!mExitMethodID) return;
	mJNIMgr.getJNIEnv().CallVoidMethod(mEngineInterface, mExitMethodID);
}
