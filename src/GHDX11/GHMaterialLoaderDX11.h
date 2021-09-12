// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHFileOpener;
class GHMaterialCallbackMgr;
class GHShaderDX11;
class GHRenderDeviceDX11;
class GHMaterial;
class GHMDesc;
class GHRenderStateMgrDX11;
class GHEventMgr;
class GHXMLObjLoaderGHM;
class GHResourceFactory;

class GHMaterialLoaderDX11 : public GHXMLObjLoader
{
public:
	GHMaterialLoaderDX11(GHResourceFactory& resourceCache,
							const GHMaterialCallbackMgr& callbackMgr,
							const GHXMLObjLoaderGHM& ghmDescLoader,
							GHRenderDeviceDX11& device,
							GHRenderStateMgrDX11& stateMgr,
							GHEventMgr& eventMgr);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
	// strip off .glsl or whatever and add .cso to the end.
	void createShaderName(const char* srcName, char* destName) const;

private:
	const GHXMLObjLoaderGHM& mDescLoader;
    const GHMaterialCallbackMgr& mCallbackMgr;
	GHRenderDeviceDX11& mDevice;
	GHResourceFactory& mResourceCache;
	GHRenderStateMgrDX11& mStateMgr;
	GHEventMgr& mEventMgr;
};
