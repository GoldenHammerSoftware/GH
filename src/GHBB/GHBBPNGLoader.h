// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"

class GHFileOpener;
class GHGLESTextureMgr;

class GHBBPNGLoader : public GHResourceLoader
{
public:
	GHBBPNGLoader(GHFileOpener& fileOpener, GHGLESTextureMgr& textureMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);

private:
	GHGLESTextureMgr& mTextureMgr;
	GHFileOpener& mFileOpener;
};
