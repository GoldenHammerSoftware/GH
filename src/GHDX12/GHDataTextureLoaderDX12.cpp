#include "GHDataTextureLoaderDX12.h"
#include "Render/GHTextureDataLoader.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHTextureData.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHTextureDX12.h"
#include "GHPlatform/win32/GHLChar.h"

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
	GHTextureDX12* ret = (GHTextureDX12*)loadMemory(data, sizeof(GHTextureData), extraData);
	wchar_t* wfilename = GHLChar::convertToWide(filename);
	ret->getDXBuffer()->SetName(wfilename);
	delete wfilename;
	return ret;
}

GHResource* GHDataTextureLoaderDX12::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
	if (memSize != sizeof(GHTextureData))
	{
		GHDebugMessage::outputString("Expecting a GHTextureData for loadMemory");
		return 0;
	}
	GHTextureData* textureData = (GHTextureData*)(mem);
	GHTextureDX12* ret = new GHTextureDX12(mDevice, textureData, true, nullptr);
	ret->deleteSourceData();
	return ret;
}
