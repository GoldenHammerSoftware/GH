// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESRenderServices.h"
#include "GHPlatform/null/GHNullMutex.h"
#include <jni.h>

class GHJNIMgr;
class GHAndroidGLESContext;
class GHFileOpener;
class GHEventMgr;

class GHAndroidRenderServices : public GHGLESRenderServices
{
public:
	GHAndroidRenderServices(int windowWidth, int windowHeight,
                            GHSystemServices& systemServices,
                            const GHStringIdFactory& hashTable,
                            GHJNIMgr& jniMgr,
                            jobject jobj,
                            const GHFileOpener& fileOpener);
	~GHAndroidRenderServices(void);

	virtual void initAppShard(GHAppShard& appShard);

private:
	GHNullMutex mMutex;
    GHAndroidGLESContext* mContext;
	GHJNIMgr& mJNIMgr;
    jobject mJObj;
    const GHFileOpener& mFileOpener;
};
