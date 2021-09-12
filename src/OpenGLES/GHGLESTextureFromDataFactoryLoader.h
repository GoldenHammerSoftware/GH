// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHTextureDataLoader.h"

class GHTextureDataFactory;
class GHGLESTextureMgr;
class GHFileOpener;
class GHGLESStateCache;

// Creates a GHGLESTexture by using a GHTextureDataFactory.
class GHGLESTextureFromDataFactoryLoader : public GHResourceLoader
{
public:
	GHGLESTextureFromDataFactoryLoader(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, const GHFileOpener& fileOpener, GHTextureDataFactory* dataFactory);
	~GHGLESTextureFromDataFactoryLoader(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;
	GHTextureDataLoader mDataLoader;
	GHTextureDataFactory* mDataFactory;
};
