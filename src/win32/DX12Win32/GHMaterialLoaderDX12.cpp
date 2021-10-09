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

GHMaterialLoaderDX12::GHMaterialLoaderDX12(GHResourceFactory& resourceCache,
	const GHMaterialCallbackMgr& callbackMgr,
	const GHXMLObjLoaderGHM& ghmDescLoader)
	: mResourceCache(resourceCache)
	, mCallbackMgr(callbackMgr)
	, mDescLoader(ghmDescLoader)
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
	return 0;
}

