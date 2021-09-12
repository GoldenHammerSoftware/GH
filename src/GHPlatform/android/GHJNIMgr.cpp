// Copyright 2010 Golden Hammer Software
#include "GHJNIMgr.h"
#include "GHPlatform/GHDebugMessage.h"

GHJNIMgr::GHJNIMgr(JNIEnv& mainThreadEnv, JavaVM* jvm, jobject activity)
: mMainThreadEnv(mainThreadEnv)
, mActivity(activity)
{
	mJVM = jvm;
}

JNIEnv& GHJNIMgr::getJNIEnv(void)
{
	//return mMainThreadEnv;

	///* // it seems like the following code should be needed, but it seems to be working anyway.
	JNIEnv* env;
	int status = mJVM->GetEnv((void**)&env, JNI_VERSION_1_6);
	if (status < 0) {
		GHDebugMessage::outputString("Failed to get JNIEnv");
	}
	return *env;
	//*/
}

JavaVM* GHJNIMgr::getJVM(void)
{
	return mJVM;
}

jobject GHJNIMgr::getActivity(void)
{
	return mActivity;
}

void GHJNIMgr::dumpReferenceTables(void)
{
	jclass vm_class = getJNIEnv().FindClass("dalvik/system/VMDebug");
	jmethodID dump_mid = getJNIEnv().GetStaticMethodID(vm_class, "dumpReferenceTables", "()V");
	getJNIEnv().CallStaticVoidMethod(vm_class, dump_mid);
}
