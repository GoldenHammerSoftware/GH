#include "GHTextureLoaderDX12.h"
#include "GHWin32/GHWin32FileFinder.h"
#include "GHString/GHFilenameUtil.h"
#include "GHUtils/GHPropertyContainer.h"
#include "Render/GHRenderProperties.h"
#include "GHPlatform/GHDebugMessage.h"
#include "Render/GHTextureFormat.h"
#include "GHWin32/GHDXGIUtil.h"
#include "GHTextureDX12.h"

GHTextureLoaderDX12::GHTextureLoaderDX12(const GHWindowsFileFinder& fileFinder)
	: mWICUtil(fileFinder)
{
}

GHTextureLoaderDX12::~GHTextureLoaderDX12(void)
{
}

GHResource* GHTextureLoaderDX12::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	void* pixels = 0;
	unsigned int width, height, depth;
	DXGI_FORMAT dxFormat;
	bool allowMipmaps = extraData ? !extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS) : true;
	bool keepTextureData = extraData ? (bool)extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA) : false;

	const char* redirects[] = {
		//"dds",
		"jpg",
		"png"
	};
	size_t numRedirects = sizeof(redirects) / sizeof(redirects[0]);
	char newName[512];
	for (size_t i = 0; i < numRedirects; ++i)
	{
		GHFilenameUtil::replaceExtension(filename, redirects[i], newName, 512);
		if (mWICUtil.createTexture(newName, &pixels, width, height, depth, dxFormat))
		{
			break;
		}
	}
	if (!pixels)
	{
		if (!mWICUtil.createTexture(filename, &pixels, width, height, depth, dxFormat))
		{
			GHDebugMessage::outputString("Failed to find texture %s", filename);
			return 0;
		}
	}

	return createGHTexture(pixels, width, height, 4, 1, allowMipmaps, dxFormat, keepTextureData);
}

GHResource* GHTextureLoaderDX12::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
	if (!extraData)
	{
		GHDebugMessage::outputString("GHTextureLoaderDX12::loadMemory called with no property container, returning 0");
		return 0;
	}

	GHTextureFormat::Enum textureFormat = extraData->getProperty(GHRenderProperties::GP_TEXTUREFORMAT);
	uint32_t width = extraData->getProperty(GHRenderProperties::GP_WIDTH);
	uint32_t height = extraData->getProperty(GHRenderProperties::GP_HEIGHT);
	uint32_t numMips = extraData->getProperty(GHRenderProperties::GP_NUMMIPS);
	bool allowMipmaps = !extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS);

	bool keepTextureData = extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);
	const char* filename = 0;

	if (textureFormat == GHTextureFormat::TF_RGB8)
	{
		// dx12 does not support rgb8 textures, we need to create a rgba8 buffer.
		return createRGBAFromRGB(mem, memSize, *extraData);
	}

	DXGI_FORMAT dxFormat = GHDXGIUtil::convertGHFormatToDXGI(textureFormat);
	return createGHTexture(mem, width, height, 4, numMips, allowMipmaps, dxFormat, keepTextureData);
}

// turn a rgb8 texture buffer into a rgba8 texture buffer and pass it on to loadMemory()
GHResource* GHTextureLoaderDX12::createRGBAFromRGB(void* mem, size_t memSize, GHPropertyContainer& extraData)
{
	GHTextureFormat::Enum textureFormat = extraData.getProperty(GHRenderProperties::GP_TEXTUREFORMAT);
	uint32_t width = extraData.getProperty(GHRenderProperties::GP_WIDTH);
	uint32_t height = extraData.getProperty(GHRenderProperties::GP_HEIGHT);
	bool keepTextureData = extraData.getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);
	assert(textureFormat == GHTextureFormat::TF_RGB8);
	extraData.setProperty(GHRenderProperties::GP_TEXTUREFORMAT, GHTextureFormat::TF_RGBA8);

	char* newMem = new char[width * height * 4];
	char* newBuff = newMem;
	char* srcBuff = (char*)mem;
	for (size_t pixelId = 0; pixelId < width * height; ++pixelId)
	{
		memcpy(newBuff, srcBuff, 3);
		srcBuff += 3;
		newBuff += 3;
		*newBuff = (char)255;
		newBuff++;
	}

	if (!keepTextureData)
	{
		delete mem;
	}
	extraData.setProperty(GHRenderProperties::GP_KEEPTEXTUREDATA, 0);
	return loadMemory(newMem, width * height * 4, &extraData);
}

GHResource* GHTextureLoaderDX12::createGHTexture(void* mem, unsigned int width, unsigned int height, unsigned int depth, unsigned int numMips, bool allowMipmaps, DXGI_FORMAT dxFormat, bool keepTextureData)
{
	// todo: load into dx
	GHResource* ret = new GHTextureDX12();
	if (!keepTextureData)
	{
		delete[] mem;
	}
	return ret;
}
