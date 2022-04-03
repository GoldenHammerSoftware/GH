#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHTextureDataLoader;
class GHRenderDeviceDX12;

// texture loader to create DX12 textures from a texture data factory.
class GHDataTextureLoaderDX12 : public GHResourceLoader
{
public:
	GHDataTextureLoaderDX12(const GHTextureDataLoader& dataLoader, GHRenderDeviceDX12& device);
	~GHDataTextureLoaderDX12(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0) override;
	virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) override;

private:
	const GHTextureDataLoader& mDataLoader;
	GHRenderDeviceDX12& mDevice;
};
