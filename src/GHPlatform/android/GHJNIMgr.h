// Copyright 2010 Golden Hammer Software
#pragma once

#include <jni.h>

// keeps track of JNI stuff to avoid threading issues
class GHJNIMgr
{
public:
	GHJNIMgr(JNIEnv& mainThreadEnv, JavaVM* jvm, jobject activity);

	JNIEnv& getJNIEnv(void);
	JavaVM* getJVM(void);
	jobject getActivity(void);

	void dumpReferenceTables(void);

private:
	JNIEnv& mMainThreadEnv;
	JavaVM* mJVM;
	jobject mActivity;
};
