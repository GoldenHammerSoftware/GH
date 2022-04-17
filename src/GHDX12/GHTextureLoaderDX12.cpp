#include "GHTextureLoaderDX12.h"
#include "GHWin32/GHWin32FileFinder.h"
#include "GHString/GHFilenameUtil.h"
#include "GHUtils/GHPropertyContainer.h"
#include "Render/GHRenderProperties.h"
#include "GHPlatform/GHDebugMessage.h"
#include "Render/GHTextureFormat.h"
#include "Render/GHDXGIUtil.h"
#include "GHTextureDX12.h"
#include "GHDX12Include.h"
#include "GHRenderDeviceDX12.h"
#include "GHDX12Helpers.h"
#include "GHMipmapGeneratorDX12.h"
#include "Render/GHTextureData.h"

GHTextureLoaderDX12::GHTextureLoaderDX12(const GHWindowsFileFinder& fileFinder, GHRenderDeviceDX12& device, GHMipmapGeneratorDX12& mipGen)
	: mWICUtil(fileFinder)
	, mMipGen(mipGen)
	, mDevice(device)
{
}

GHTextureLoaderDX12::~GHTextureLoaderDX12(void)
{
}

GHResource* GHTextureLoaderDX12::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	// first look for file format overrides
	for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
	{
		GHResourceLoader* loader = *iter;
		GHResource* overRet = loader->loadFile(filename);
		if (overRet) {
			return overRet;
		}
	}

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

	GHTextureData* textureData = createTextureData(pixels, width, height, 4, dxFormat);
	GHTextureDX12* ret = new GHTextureDX12(mDevice, textureData, allowMipmaps, &mMipGen);

	if (!keepTextureData)
	{
		ret->deleteSourceData();
	}
	return ret;
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

	DXGI_FORMAT dxFormat = (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureFormat);
	GHTextureData* textureData = createTextureData(mem, width, height, 4, dxFormat);
	GHTextureDX12* ret = new GHTextureDX12(mDevice, textureData, allowMipmaps, &mMipGen);

	if (!keepTextureData)
	{
		ret->deleteSourceData();
	}
	return ret;
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

GHTextureData* GHTextureLoaderDX12::createTextureData(void* mem, unsigned int width, unsigned int height, unsigned int depth, DXGI_FORMAT dxFormat)
{
	GHTextureData* ret = new GHTextureData();
	ret->mTextureFormat = GHDXGIUtil::convertDXGIFormatToGH((GHDXGIFormat)dxFormat);
	ret->mTextureType = GHTextureType::TT_2D;
	ret->mSrgb = false; // todo.
	ret->mChannelType = GHTextureChannelType::TC_FLOAT; // always uncompressed here.
	ret->mNumSlices = 1;
	ret->mDataSource = (int8_t*)mem;
	ret->mDepth = depth;

	ret->mMipLevels.resize(1);
	ret->mMipLevels[0].mData = ret->mDataSource;
	ret->mMipLevels[0].mHeight = height;
	ret->mMipLevels[0].mWidth = width;
	ret->mMipLevels[0].mDataSize = width * height * depth * sizeof(float);
	return ret;
}

void GHTextureLoaderDX12::addOverrideLoader(GHResourceLoader* loader)
{
	mOverrideLoaders.push_back(loader);
}
