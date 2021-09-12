// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"
#include "GHCocoaFileNameCreator.h"

class GHOGLTextureMgr;
class GHMutex;

class GHCocoaTextureLoader : public GHResourceLoader
{
public:
    GHCocoaTextureLoader(GHOGLTextureMgr& textureMgr, GHMutex& renderMutex);
    
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
    
private:
    GHMutex& mRenderMutex;
    GHOGLTextureMgr& mTextureMgr;
	GHCocoaFileNameCreator mFileNameCreator;
	bool mSupportsDDS;
};
