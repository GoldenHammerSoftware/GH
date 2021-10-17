#pragma once

#include "Base/GHXMLObjLoader.h"

class GHResourceFactory;
class GHMaterialCallbackMgr;
class GHXMLObjLoaderGHM;
class GHRenderDeviceDX12;

class GHMaterialLoaderDX12 : public GHXMLObjLoader
{
public:
	GHMaterialLoaderDX12(GHRenderDeviceDX12& device, 
		GHResourceFactory& resourceCache,
		const GHMaterialCallbackMgr& callbackMgr,
		const GHXMLObjLoaderGHM& ghmDescLoader);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const override;
	GH_NO_POPULATE

private:
	GHRenderDeviceDX12& mDevice;
	const GHXMLObjLoaderGHM& mDescLoader;
	const GHMaterialCallbackMgr& mCallbackMgr;
	GHResourceFactory& mResourceCache;
};

