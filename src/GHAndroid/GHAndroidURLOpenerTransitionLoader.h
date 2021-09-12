// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include <jni.h>
#include "GHPlatform/android/GHJNIMgr.h"

class GHAndroidURLOpenerTransitionLoader : public GHXMLObjLoader
{
public:
    GHAndroidURLOpenerTransitionLoader(GHJNIMgr& jniMgr, jobject jObject);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
	jobject mJavaObj;
	GHJNIMgr& mJNIMgr;
};
