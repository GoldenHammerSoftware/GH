// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHString/GHIdentifierMap.h"
#include "GHShaderParamListDX11.h"
#include "GHUtils/GHResource.h"
#include "GHString/GHString.h"

class GHShaderDX11;
class GHFileOpener;
class GHRenderDeviceDX11;
class GHXMLSerializer;
class GHXMLNode;
class GHEventMgr;

class GHShaderLoaderDX11 : public GHResourceLoader
{
public:
	GHShaderLoaderDX11(GHFileOpener& fileOpener, GHRenderDeviceDX11& device, 
		GHShaderDX11*& activeVS, GHXMLSerializer& xmlSerializer, 
		const GHIdentifierMap<int>& enumStore, const int& graphicsQuality, 
		const char* shaderDirectory, GHEventMgr& eventMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);

private:
	GHShaderDX11* loadVS(const char* shaderName, GHResourcePtr<GHShaderParamListDX11>* params) const;
	GHShaderDX11* loadPS(const char* shaderName, GHResourcePtr<GHShaderParamListDX11>* params) const;
	GHResourcePtr<GHShaderParamListDX11>* loadParams(const char* shaderName) const;
	void loadSharedArguments(GHShaderParamListDX11::Param& param, const GHXMLNode& node) const;

private:
	GHFileOpener& mFileOpener;
	GHRenderDeviceDX11& mDevice;
	GHShaderDX11*& mActiveVS;
	GHXMLSerializer& mXMLSerializer;
	const GHIdentifierMap<int>& mEnumStore;
	// a reference to the graphics quality on the render services.
	// this value is passed to multishader.
	const int& mGraphicsQuality;
	// offset location where shaders live
	GHString mShaderDirectory;
	GHEventMgr& mEventMgr;
};
