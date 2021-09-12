// Copyright 2010 Golden Hammer Software
#include "GHAndroidURLOpenerTransition.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHAndroidURLOpenerTransition::GHAndroidURLOpenerTransition(GHJNIMgr& jniMgr, jobject jObject, const char* url)
: mJNIMgr(jniMgr)
, mJavaObj(jObject)
, mOpenURLMethod(0)
, mURLString(url, GHString::CHT_COPY)
{
	jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJavaObj);
	if (cls == 0) {
		GHDebugMessage::outputString("GHAndroidURLOpener can't find class where openURL java method is supposed to exist.");
	}

	mOpenURLMethod = mJNIMgr.getJNIEnv().GetMethodID(cls, "openURL", "(Ljava/lang/String;)V");
	if (mOpenURLMethod == 0) {
		GHDebugMessage::outputString("GHAndroidURLOpener can't find openURL method");
	}
}

void GHAndroidURLOpenerTransition::activate(void)
{
	jstring jURL = mJNIMgr.getJNIEnv().NewStringUTF(mURLString.getChars());
	mJNIMgr.getJNIEnv().CallVoidMethod(mJavaObj, mOpenURLMethod, jURL);
	mJNIMgr.getJNIEnv().DeleteLocalRef(jURL);
}
