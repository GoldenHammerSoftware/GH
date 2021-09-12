// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHiOSFilenameCreator.h"

class GHMutex;
class GHGLESTextureMgr;
class GHFileOpener;
class GHGLESStateCache;

class GHiOSTextureLoaderPVRTC : public GHResourceLoader
{
public:
	GHiOSTextureLoaderPVRTC(GHGLESTextureMgr& textureMgr, GHMutex* mutex,
                            const GHFileOpener& fileOpener, GHGLESStateCache& stateCache);
	
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
	
protected:
	GHGLESTextureMgr& mTextureMgr;
	GHiOSFilenameCreator mFileNameCreator;
	GHMutex* mMutex;
    const GHFileOpener& mFileOpener;
    GHGLESStateCache& mStateCache;
};
