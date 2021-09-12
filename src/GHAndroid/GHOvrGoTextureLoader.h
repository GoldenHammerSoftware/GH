// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHGLESTextureMgr;
class GHGLESStateCache;

class GHOvrGoTextureLoader : public GHResourceLoader
{
public:
	GHOvrGoTextureLoader(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache);
	~GHOvrGoTextureLoader(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0);
	virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData);

private:
	GHResource * loadTextureFromBuffer(char* buffer, int bufferSize);

private:
	GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;
};
