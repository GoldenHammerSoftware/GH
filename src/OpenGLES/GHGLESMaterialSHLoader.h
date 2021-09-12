// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHGLESShaderMgr.h"

class GHResourceFactory;
class GHGLESShaderProgram;
class GHFileOpener;
class GHMaterialCallbackMgr;
class GHEventMgr;
class GHXMLObjLoaderGHM;
class GHStringIdFactory;
class GHGLESStateCache;

class GHGLESMaterialSHLoader : public GHXMLObjLoader
{
public:
    GHGLESMaterialSHLoader(GHResourceFactory& resourceCache,
        const GHFileOpener& fileOpener,
        const GHMaterialCallbackMgr& callbackMgr,
        const GHXMLObjLoaderGHM& ghmDescLoader,
        const GHStringIdFactory& stringHasher,
        GHEventMgr& eventMgr,
        GHGLESShaderMgr& shaderMgr,
        GHGLESStateCache& stateCache,
        const int& graphicsQuality);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    GHGLESShaderProgram* loadShaderProgram(const char* vertexStr, const char* pixelStr) const;
    
private:
    GHGLESShaderMgr& mShaderMgr;
    GHResourceFactory& mResourceFactory;
    const GHXMLObjLoaderGHM& mDescLoader;
    const GHFileOpener& mFileOpener;
    const GHMaterialCallbackMgr& mCallbackMgr;
	const GHStringIdFactory& mStringHasher;
    GHEventMgr& mEventMgr;
	GHGLESStateCache& mStateCache;
    const int& mGraphicsQuality;
};
