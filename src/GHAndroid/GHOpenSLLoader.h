// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include <jni.h>

class GHJNIMgr;

class GHOpenSLLoader : public GHResourceLoader
{
public:
	GHOpenSLLoader(GHJNIMgr& jniMgr, jobject jAssetMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	GHJNIMgr& mJNIMgr;
	jobject mJAssetMgr;
};
