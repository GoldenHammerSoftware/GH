// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include <jni.h>
#include "GHAndroidTextureFactory.h"
#include <vector>

class GHFileOpener;
class GHJNIMgr;
class GHGLESStateCache;

class GHAndroidBitmapLoader : public GHResourceLoader
{
public:
	GHAndroidBitmapLoader(GHJNIMgr& jniMgr, jobject jobj, const GHFileOpener& fileOpener,
		GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache);
	~GHAndroidBitmapLoader(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0);

	void addOverrideLoader(GHResourceLoader* loaderToOwn);

private:
	GHResource * loadTextureFromBuffer(char* buffer, int bufferSize);

private:
	GHAndroidTextureFactory mTextureFactory;
	GHJNIMgr& mJNIMgr;
	jobject mJObject;
	jmethodID mMethodID;
	const GHFileOpener& mFileOpener;

	// A list of override loaders that look for more optimal formats before defaulting to the uncompressed java bitmap.
	std::vector<GHResourceLoader*> mOverrideLoaders;
};
