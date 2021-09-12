// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHGLESInclude.h"
#include <vector>

class GHFileOpener;
class GHEventMgr;

class GHGLESShaderLoader : public GHResourceLoader
{
public:
    GHGLESShaderLoader(const GHFileOpener& fileOpener, GHEventMgr& eventMgr);
	
	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);
    
    unsigned int loadShader(const char* filename, GLenum type, bool noSpam, bool& expectsDXLayout);

private:
    const GHFileOpener& mFileOpener;
    GHEventMgr& mEventMgr;
};
