#pragma once

#include "Base/GHXMLObjLoader.h"

class GHResourceFactory;
class GHMaterialCallbackMgr;
class GHXMLObjLoaderGHM;

class GHMaterialLoaderDX12 : public GHXMLObjLoader
{
public:
	GHMaterialLoaderDX12(GHResourceFactory& resourceCache,
		const GHMaterialCallbackMgr& callbackMgr,
		const GHXMLObjLoaderGHM& ghmDescLoader);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const override;
	GH_NO_POPULATE

private:
	const GHXMLObjLoaderGHM& mDescLoader;
	const GHMaterialCallbackMgr& mCallbackMgr;
	GHResourceFactory& mResourceCache;
};

