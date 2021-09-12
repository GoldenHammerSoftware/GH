// Copyright 2020 Golden Hammer Software
#include "GHAndroidKeyboardTransitionXMLLoader.h"
#include "GHAndroidKeyboardTransition.h"

GHAndroidKeyboardTransitionXMLLoader::GHAndroidKeyboardTransitionXMLLoader(GHJNIMgr& jniMgr, jobject engineInterface)
	: mJNIMgr(jniMgr)
	, mEngineInterface(engineInterface) 
{
}

void* GHAndroidKeyboardTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	return new GHAndroidKeyboardTransition(mJNIMgr, mEngineInterface);
}
