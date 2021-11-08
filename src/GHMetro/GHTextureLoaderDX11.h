// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHTextureCreatorWICDX11.h"

class GHRenderDeviceDX11;
class GHResourceFactory;
class GHWindowsFileFinder;
class GHEventMgr;
class GHTextureDX11;

// uses WIC to load textures, not for use with DDS
class GHTextureLoaderDX11 : public GHResourceLoader
{
public:
	GHTextureLoaderDX11(GHRenderDeviceDX11& device, GHResourceFactory& resourceCache,
		const GHWindowsFileFinder& fileOpener, GHEventMgr& eventMgr);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData);

	GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) override;

private:
	GHResource* redirectPVRToPNG(const char* filename, GHPropertyContainer* extraData);
	GHResource* loadFileNoRecurse(const char* filename, GHPropertyContainer* extraData);
	GHTextureDX11* createGHTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView,
		DXGI_FORMAT dxFormat, const char* filename, void* pixels, unsigned int width, unsigned int height, unsigned int depth,
		GHPropertyContainer* extraData);
	GHResource* createRGBAFromRGB(void* mem, size_t memSize, GHPropertyContainer& extraData);

private:
	GHRenderDeviceDX11& mDevice;
	GHResourceFactory& mResourceCache;
	GHEventMgr& mEventMgr;
	GHTextureCreatorWICDX11 mTextureCreator;
};
