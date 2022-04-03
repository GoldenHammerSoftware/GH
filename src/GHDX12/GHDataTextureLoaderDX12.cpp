#include "GHDataTextureLoaderDX12.h"
#include "Render/GHTextureDataLoader.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHTextureData.h"

GHDataTextureLoaderDX12::GHDataTextureLoaderDX12(const GHTextureDataLoader& dataLoader, GHRenderDeviceDX12& device)
	: mDataLoader(dataLoader)
	, mDevice(device)
{
}
GHDataTextureLoaderDX12::~GHDataTextureLoaderDX12(void)
{
}

GHResource* GHDataTextureLoaderDX12::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHTextureData* data = mDataLoader.load(filename);
	if (!data) {
		return 0;
	}
	return loadMemory(data, sizeof(GHTextureData), extraData);
}

GHResource* GHDataTextureLoaderDX12::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
	return 0;
}
