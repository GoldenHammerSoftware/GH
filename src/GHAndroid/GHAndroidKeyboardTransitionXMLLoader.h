// Copyright 2020 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <jni.h>
class GHJNIMgr;

class GHAndroidKeyboardTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHAndroidKeyboardTransitionXMLLoader(GHJNIMgr& jniMgr, jobject engineInterface);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHJNIMgr& mJNIMgr;
	jobject mEngineInterface;
};
