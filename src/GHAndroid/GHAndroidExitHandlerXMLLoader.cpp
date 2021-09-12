// Copyright 2010 Golden Hammer Software
#include "GHAndroidExitHandlerXMLLoader.h"
#include "GHAndroidExitHandler.h"

GHAndroidExitHandlerXMLLoader::GHAndroidExitHandlerXMLLoader(GHJNIMgr& jniMgr, jobject engineInterface)
: mJNIMgr(jniMgr)
, mEngineInterface(engineInterface)
{
}

GHAndroidExitHandlerXMLLoader::~GHAndroidExitHandlerXMLLoader(void)
{
}

void* GHAndroidExitHandlerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	return new GHAndroidExitHandler(mJNIMgr, mEngineInterface);
}