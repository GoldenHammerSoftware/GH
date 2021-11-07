// Copyright Golden Hammer Software
#include "GHTextureLoaderDDS.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHTextureDX11.h"
#include "GHDX11Include.h"
#include "GHRenderDeviceDX11.h"
#include "DDSTextureLoader.h"
#include "GHUtils/GHProfiler.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHRenderProperties.h"

GHTextureLoaderDDS::GHTextureLoaderDDS(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener,
									   GHEventMgr& eventMgr)
: mDevice(device)
, mEventMgr(eventMgr)
, mTextureCreator(device, fileOpener)
{
}

GHResource* GHTextureLoaderDDS::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHPROFILEBEGIN("DDSLoad");

    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dtex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

	bool dontUseMipmaps = false;
	if (extraData)
	{
		dontUseMipmaps = extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS);
	}

	void* pixels = 0;
	unsigned int width, height, depth;
	DXGI_FORMAT dxFormat;
	if (!mTextureCreator.createTexture(filename, d3dtex, textureView, &pixels, width, height, depth, !dontUseMipmaps, dxFormat))
	{
		return 0;
	}
	if (pixels) 
	{
		// not respecting extra data keep pixels for dds files.
		// why? probably just never needed it.
		delete [] pixels;
		pixels = 0;
	}

	GHTextureDX11* ret = new GHTextureDX11(mDevice, d3dtex, textureView, &mEventMgr,
		filename, &mTextureCreator, pixels, width, height, depth, !dontUseMipmaps, dxFormat);
	GHPROFILEEND("DDSLoad");
	return ret;
}
