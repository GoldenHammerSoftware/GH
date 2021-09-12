// Copyright 2010 Golden Hammer Software
#include "GHGLESMaterialSHLoader.h"
#include "GHMDesc.h"
#include "GHGLESMaterialSH.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESInclude.h"
#include "GHGLESShaderProgram.h"
#include "GHVertexComponent.h"
#include "GHPlatform/GHFile.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHMaterialCallbackMgr.h"
#include <assert.h>
#include "GHUtils/GHProfiler.h"
#include "GHGLESShader.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGLESIdentifiers.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHXMLObjLoaderGHM.h"
#include "GHGLESErrorReporter.h"

GHGLESMaterialSHLoader::GHGLESMaterialSHLoader(GHResourceFactory& resourceCache,
    const GHFileOpener& fileOpener,
    const GHMaterialCallbackMgr& callbackMgr,
    const GHXMLObjLoaderGHM& ghmDescLoader,
    const GHStringIdFactory& stringHasher,
    GHEventMgr& eventMgr,
    GHGLESShaderMgr& shaderMgr,
    GHGLESStateCache& stateCache,
    const int& graphicsQuality)
: mShaderMgr(shaderMgr)
, mDescLoader(ghmDescLoader)
, mFileOpener(fileOpener)
, mCallbackMgr(callbackMgr)
, mResourceFactory(resourceCache)
, mEventMgr(eventMgr)
, mStringHasher(stringHasher)
, mStateCache(stateCache)
, mGraphicsQuality(graphicsQuality)
{
}

void* GHGLESMaterialSHLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* desc = (GHMDesc*)mDescLoader.create(node, extraData);
    if (!desc) return 0;
    
    //GHDebugMessage::outputString("Loading shader %s %s", desc->mVertexFile.getChars(), desc->mPixelFile.getChars());
    GHGLESShaderProgram* shader = loadShaderProgram(desc->mVertexFile, desc->mPixelFile);
    if (!shader)
    {
        GHDebugMessage::outputString("Failed to load shader %s %s", desc->mVertexFile.getChars(), desc->mPixelFile.getChars());
        delete desc;
        return 0;
    }
    GHGLESMaterialSH* ret = new GHGLESMaterialSH(shader, desc, mEventMgr, mStateCache);
    mCallbackMgr.createCallbacks(*ret);
    return ret;
}

void GHGLESMaterialSHLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    assert(false);
}

GHGLESShaderProgram* GHGLESMaterialSHLoader::loadShaderProgram(const char* vertexStr, const char* pixelStr) const
{
    if (!vertexStr || !pixelStr) {
        GHDebugMessage::outputString("loadShaderProgram called with missing args.");
        return 0;   
    }
    GHPROFILESCOPE("loadShaderProgram", GHString::CHT_REFERENCE)

    unsigned int retId = glCreateProgram();
	if (retId == 0) {
	    GHGLESErrorReporter::checkAndReportError("glCreateProgram");
	    GHDebugMessage::outputString("Failed to create empty shader program.");
	    return 0;
	}

    GHPropertyContainer loadingArgs;
    loadingArgs.setProperty(GHGLESIdentifiers::SHADERTYPE, (unsigned int)GL_VERTEX_SHADER);
    GHGLESShader* vertShader = (GHGLESShader*)mResourceFactory.getCacheResource(vertexStr, &loadingArgs);
    if (!vertShader) {
        GHDebugMessage::outputString("Failed to get cache resource for vertex shader %s.", vertexStr);
        return 0;
    }
    loadingArgs.setProperty(GHGLESIdentifiers::SHADERTYPE, (unsigned int)GL_FRAGMENT_SHADER);
    GHGLESShader* pixelShader = (GHGLESShader*)mResourceFactory.getCacheResource(pixelStr, &loadingArgs);
    if (!pixelShader) {
        GHDebugMessage::outputString("Failed to get cache resource for pixel shader %s.", pixelStr);
        vertShader->acquire(); vertShader->release();
        return 0;
    }
	
    if (!GHGLESShaderProgram::linkShaders(retId, vertShader, pixelShader))
    {
        GHDebugMessage::outputString("Failed to link shaders %s %s", vertexStr, pixelStr);
        // don't return 0 on a failed link.  It could mean that we're loading after
        // an interrupt has happened but before the device reinit happens.
        // if that's the case then we will be fine after the reinit.
        //return 0;
    }
	
	return new GHGLESShaderProgram(retId, vertShader, pixelShader, mShaderMgr, mStringHasher, mGraphicsQuality);
}
