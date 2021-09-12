// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"

class GHResourceFactory;
class GHFileOpener;
class GHGLESTextureMgr;

class GHBBJPGLoader : public GHResourceLoader
{
public:
	GHBBJPGLoader(GHFileOpener& fileOpener, GHResourceFactory& resourceCache,
			GHGLESTextureMgr& textureMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);

private:
	GHResource* redirectPVRToJPG(const char* filename);

private:
	GHFileOpener& mFileOpener;
	GHResourceFactory& mResourceCache;
	GHGLESTextureMgr& mTextureMgr;
};
