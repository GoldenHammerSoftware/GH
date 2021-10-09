// Copyright Golden Hammer Software
#include "GHMaterialLoaderDX11.h"
#include "GHMDesc.h"
#include "GHMaterialDX11.h"
#include "GHMaterialCallbackMgr.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX11.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPropertiesDX11.h"
#include "GHMaterialParamHandle.h"
#include "GHXMLObjLoaderGHM.h"
#include <stdlib.h>
#include "GHRenderProperties.h"

GHMaterialLoaderDX11::GHMaterialLoaderDX11(GHResourceFactory& resourceCache,
                                             const GHMaterialCallbackMgr& callbackMgr,
                                             const GHXMLObjLoaderGHM& ghmDescLoader,
											 GHRenderDeviceDX11& device,
											 GHRenderStateMgrDX11& stateMgr,
											 GHEventMgr& eventMgr)
: mDescLoader(ghmDescLoader)
, mCallbackMgr(callbackMgr)
, mDevice(device)
, mResourceCache(resourceCache)
, mStateMgr(stateMgr)
, mEventMgr(eventMgr)
{
}

static void createShaderName(const char* srcName, char* destName)
{
	const char* extStart = ::strrchr(srcName, '.');
	if (!extStart) {
		::snprintf(destName, 1024, "%s.cso", srcName);
		return;
	}
	::snprintf(destName, extStart - srcName + 1, "%s", srcName);
	destName[extStart - srcName] = '\0';
	::snprintf(destName, 1024, "%s.cso", destName);
}

void* GHMaterialLoaderDX11::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHMDesc* desc = (GHMDesc*)mDescLoader.create(node, extraData);
    if (!desc) return 0;

	char nameBuffer[1024];
	if (desc->mVertexFile) createShaderName(desc->mVertexFile, nameBuffer);
	else ::sprintf(nameBuffer, "DefaultVertex.cso");
	extraData.setProperty(GHPropertiesDX11::SHADERTYPE, 0);
	GHShaderResource* vs = (GHShaderResource*)mResourceCache.getCacheResource(nameBuffer, &extraData);
	if (!vs) {
		GHDebugMessage::outputString("Failed to load vs %s", nameBuffer);
		delete desc;
		return 0;
	}

	if (desc->mPixelFile) createShaderName(desc->mPixelFile, nameBuffer);
	else ::sprintf(nameBuffer, "DefaultPixel.cso");
	extraData.setProperty(GHPropertiesDX11::SHADERTYPE, 1);
	GHShaderResource* ps = (GHShaderResource*)mResourceCache.getCacheResource(nameBuffer, &extraData);
	if (!ps) {
		GHDebugMessage::outputString("Failed to load ps %s", nameBuffer);
		delete desc;
		vs->acquire();
		vs->release();
		return 0;
	}

	bool isOverrideMat = extraData.getProperty(GHRenderProperties::GP_LOADINGMATERIALOVERRIDE);
	GHMaterialDX11* ret = new GHMaterialDX11(mDevice, desc, vs, ps, mStateMgr, mEventMgr, isOverrideMat);
    mCallbackMgr.createCallbacks(*ret);
    return ret;
}

