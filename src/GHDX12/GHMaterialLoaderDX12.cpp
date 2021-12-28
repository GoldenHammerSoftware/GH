#include "GHMaterialLoaderDX12.h"
#include "Render/GHMDesc.h"
#include "GHMaterialDX12.h"
#include "Render/GHMaterialCallbackMgr.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHUtils/GHPropertyContainer.h"
#include "Render/GHMaterialParamHandle.h"
#include "Render/GHXMLObjLoaderGHM.h"
#include <stdlib.h>
#include "Render/GHRenderProperties.h"
#include "GHShaderDX12.h"
#include "GHDX12MaterialHeapPool.h"

GHMaterialLoaderDX12::GHMaterialLoaderDX12(GHRenderDeviceDX12& device, 
	GHResourceFactory& resourceCache,
	const GHMaterialCallbackMgr& callbackMgr,
	const GHXMLObjLoaderGHM& ghmDescLoader,
	GHDX12MaterialHeapPool& descriptorHeapPool)
	: mDevice(device)
	, mResourceCache(resourceCache)
	, mCallbackMgr(callbackMgr)
	, mDescLoader(ghmDescLoader)
	, mDescriptorHeapPool(descriptorHeapPool)
	, mPSOPool(device)
{
}

static void createShaderName(const char* srcName, char* destName)
{
	// duplicated with dx11, but dx11 is deprecated.
	const char* extStart = ::strrchr(srcName, '.');
	if (!extStart) {
		::snprintf(destName, 1024, "%s.cso", srcName);
		return;
	}
	::snprintf(destName, extStart - srcName + 1, "%s", srcName);
	destName[extStart - srcName] = '\0';
	::snprintf(destName, 1024, "%s.cso", destName);
}

void* GHMaterialLoaderDX12::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHMDesc* desc = (GHMDesc*)mDescLoader.create(node, extraData);
	if (!desc) return 0;

	char nameBuffer[1024];
	if (desc->mVertexFile) createShaderName(desc->mVertexFile, nameBuffer);
	else ::sprintf(nameBuffer, "DefaultVertex.cso");
	GHShaderResource* vs = (GHShaderResource*)mResourceCache.getCacheResource(nameBuffer, &extraData);
	if (!vs) {
		GHDebugMessage::outputString("Failed to load vs %s", nameBuffer);
		delete desc;
		return 0;
	}

	if (desc->mPixelFile) createShaderName(desc->mPixelFile, nameBuffer);
	else ::sprintf(nameBuffer, "DefaultPixel.cso");
	GHShaderResource* ps = (GHShaderResource*)mResourceCache.getCacheResource(nameBuffer, &extraData);
	if (!ps) {
		GHDebugMessage::outputString("Failed to load ps %s", nameBuffer);
		delete desc;
		vs->acquire();
		vs->release();
		return 0;
	}

	// dx11 checked overridemat here to squelch some errors.
	//bool isOverrideMat = extraData.getProperty(GHRenderProperties::GP_LOADINGMATERIALOVERRIDE);

	GHMaterialDX12* ret = new GHMaterialDX12(mDevice, (GHDX12MaterialHeapPool&)mDescriptorHeapPool, (GHDX12PSOPool&)mPSOPool, desc, vs, ps);
	mCallbackMgr.createCallbacks(*ret);
	return ret;
}

