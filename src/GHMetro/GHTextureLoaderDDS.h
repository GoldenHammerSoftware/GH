// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHTextureCreatorDDS.h"

class GHRenderDeviceDX11;
class GHWindowsFileFinder;
class GHEventMgr;

class GHTextureLoaderDDS : public GHResourceLoader
{
public:
	GHTextureLoaderDDS(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener,
		GHEventMgr& eventMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

private:
	GHTextureCreatorDDS mTextureCreator;
	GHRenderDeviceDX11& mDevice;
	GHEventMgr& mEventMgr;
};
