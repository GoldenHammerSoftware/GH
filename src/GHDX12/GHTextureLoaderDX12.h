#pragma once

#include "GHUtils/GHResourceLoader.h"
#include "GHWin32/GHWin32WICUtil.h"
#include <vector>
#include "GHDX12Include.h"

class GHWindowsFileFinder;
class GHRenderDeviceDX12;
class GHMipmapGeneratorDX12;
struct GHTextureData;

class GHTextureLoaderDX12 : public GHResourceLoader
{
public:
	GHTextureLoaderDX12(const GHWindowsFileFinder& fileFinder, GHRenderDeviceDX12& device, GHMipmapGeneratorDX12& mipGen);
	~GHTextureLoaderDX12(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0) override;
	virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) override;

	void addOverrideLoader(GHResourceLoader* loader);

private:
	// turn a rgb8 texture buffer into a rgba8 texture buffer and pass it on to loadMemory()
	GHResource* createRGBAFromRGB(void* mem, size_t memSize, GHPropertyContainer& extraData);

	GHTextureData* createTextureData(void* mem, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT dxFormat);
	Microsoft::WRL::ComPtr<ID3D12Resource> createDXTexture(const GHTextureData& textureData, bool generateMipmaps);

private:
	GHRenderDeviceDX12& mDevice;
	GHMipmapGeneratorDX12& mMipGen;
	GHWin32WICUtil mWICUtil;
	std::vector<GHResourceLoader*> mOverrideLoaders;
};
