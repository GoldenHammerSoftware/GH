// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <jni.h>

class GHJNIMgr;

class GHAndroidExitHandlerXMLLoader : public GHXMLObjLoader
{
public:
	GHAndroidExitHandlerXMLLoader(GHJNIMgr& jniMgr, jobject engineInterface);
	virtual ~GHAndroidExitHandlerXMLLoader(void);
    
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
    GHJNIMgr& mJNIMgr;
    jobject mEngineInterface;
};
