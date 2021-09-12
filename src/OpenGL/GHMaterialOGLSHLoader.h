// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHOGLShaderProgram;
class GHFileOpener;
class GHMaterialCallbackMgr;
class GHXMLObjLoaderGHM;

class GHMaterialOGLSHLoader : public GHXMLObjLoader
{
public:
    GHMaterialOGLSHLoader(const GHFileOpener& fileOpener,
                          const GHMaterialCallbackMgr& callbackMgr,
                          GHXMLObjLoaderGHM& descLoader);
   
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    GHOGLShaderProgram* loadShaderProgram(const char* vertexStr, const char* pixelStr) const;
    
private:
    GHXMLObjLoaderGHM& mDescLoader;
    const GHFileOpener& mFileOpener;
    const GHMaterialCallbackMgr& mCallbackMgr;
};
