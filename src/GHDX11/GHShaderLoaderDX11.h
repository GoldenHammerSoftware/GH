// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHShaderParamList.h"
#include "GHUtils/GHResource.h"
#include "GHString/GHString.h"
#include "Render/GHShaderParamListLoader.h"

class GHShaderDX11;
class GHFileOpener;
class GHRenderDeviceDX11;
class GHEventMgr;

class GHShaderLoaderDX11 : public GHResourceLoader
{
public:
	GHShaderLoaderDX11(GHFileOpener& fileOpener, GHRenderDeviceDX11& device, 
		GHShaderDX11*& activeVS, const GHShaderParamListLoader& paramListLoader, const int& graphicsQuality,
		const char* shaderDirectory, GHEventMgr& eventMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);

private:
	GHShaderDX11* loadVS(const char* shaderName, GHResourcePtr<GHShaderParamList>* params) const;
	GHShaderDX11* loadPS(const char* shaderName, GHResourcePtr<GHShaderParamList>* params) const;

private:
	GHFileOpener& mFileOpener;
	GHRenderDeviceDX11& mDevice;
	GHShaderDX11*& mActiveVS;
	// a reference to the graphics quality on the render services.
	// this value is passed to multishader.
	const int& mGraphicsQuality;
	// offset location where shaders live
	GHString mShaderDirectory;
	GHEventMgr& mEventMgr;
	const GHShaderParamListLoader& mParamListLoader;
};
