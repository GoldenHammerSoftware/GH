// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHiOSFilenameCreator.h"
#include <vector>

class GHMutex;
class GHGLESTextureMgr;
class GHGLESStateCache;
class GHResourceLoader;
class GHResourceFactory;

// loads textures for ios
// also redirects any texture to be pvr if possible.
class GHiOSTextureLoader : public GHResourceLoader
{
public:
	GHiOSTextureLoader(GHGLESTextureMgr& textureMgr, GHMutex* mutex,
                       GHResourceFactory& cache, GHGLESStateCache& stateCache);
    ~GHiOSTextureLoader(void);
    
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

    void addOverrideLoader(GHResourceLoader* loader);
    
protected:
    std::vector<GHResourceLoader*> mOverrideLoaders;
    
	GHGLESTextureMgr& mTextureMgr;
    GHGLESStateCache& mStateCache;
	GHiOSFilenameCreator mFileNameCreator;
	GHMutex* mMutex;    
};
