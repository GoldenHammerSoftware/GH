// Copyright Golden Hammer Software
#include "GHTextureLoaderDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX11/GHTextureDX11.h"
#include "GHWin32/GHWindowsFileFinder.h"
#include "GHDX11/GHDX11Include.h"
#include "GHDX11/GHRenderDeviceDX11.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHUtils/GHProfiler.h"
#include "GHUtils/GHPropertyContainer.h"
#include "Render/GHRenderProperties.h"
#include "Render/GHTextureFormat.h"
#include "Render/GHDXGIUtil.h"

GHTextureLoaderDX11::GHTextureLoaderDX11(GHRenderDeviceDX11& device, GHResourceFactory& resourceCache, 
	const GHWindowsFileFinder& fileOpener, GHEventMgr& eventMgr)
: mDevice(device)
, mResourceCache(resourceCache)
, mEventMgr(eventMgr)
, mTextureCreator(device, fileOpener)
{
}

GHResource* GHTextureLoaderDX11::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHResource* redirectRet = redirectPVRToPNG(filename, extraData);
	if (redirectRet) return redirectRet;
	GHResource* ret = loadFileNoRecurse(filename, extraData);
	if (!ret) {
		GHDebugMessage::outputString("Failed to find texture %s", filename);
	}
	return ret;
}

GHResource* GHTextureLoaderDX11::loadFileNoRecurse(const char* filename, GHPropertyContainer* extraData)
{
	GHPROFILEBEGIN("PNGLoad");

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
		GHPROFILEEND("PNGLoad");
		return 0;
	}
	GHTextureDX11* ret = createGHTexture(d3dtex, textureView, dxFormat, filename, pixels, width, height, depth, extraData);

	GHPROFILEEND("PNGLoad");
	return ret;
}

GHTextureDX11* GHTextureLoaderDX11::createGHTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView, DXGI_FORMAT dxFormat,
	const char* filename, void* pixels, unsigned int width, unsigned int height, unsigned int depth, GHPropertyContainer* extraData)
{
	if (pixels) {
		if (!extraData || (int)extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA) == 0)
		{
			delete[] pixels;
			pixels = 0;
		}
		else {
			GHDebugMessage::outputString("Keeping pixels for %s", filename);
		}
	}

	bool dontUseMipmaps = false;
	if (extraData)
	{
		dontUseMipmaps = extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS);
	}

	GHTextureDX11* ret = new GHTextureDX11(mDevice, d3dTex, textureView, &mEventMgr,
		filename, &mTextureCreator, pixels, width, height, depth, !dontUseMipmaps, dxFormat);

	return ret;
}

GHResource* GHTextureLoaderDX11::redirectPVRToPNG(const char* filename, GHPropertyContainer* extraData)
{
	const char* ext = ::strstr(filename, ".");
	if (ext == 0) {
		return 0;
	}

	char newName[128];
	::strcpy(newName, filename);
	char* ext2 = ::strstr(newName, ".");
	assert(ext2 != nullptr);
	if (ext2 == nullptr) return 0; // quiet warnings.

	*(ext2+1) = 'd'; *(ext2+2) = 'd'; *(ext2+3) = 's'; *(ext2+4) = '\0';
	GHResource* ret = mResourceCache.getUniqueResource(newName, extraData);
	if (ret) return ret;

	*(ext2+1) = 'j'; *(ext2+2) = 'p'; *(ext2+3) = 'g'; *(ext2+4) = '\0';
	ret = loadFileNoRecurse(newName, extraData);
	if (ret) return ret;

	*(ext2+1) = 'p'; *(ext2+2) = 'n'; *(ext2+3) = 'g'; *(ext2+4) = '\0';
	ret = loadFileNoRecurse(newName, extraData);
	return ret;
}

GHResource* GHTextureLoaderDX11::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
	GHPROFILESCOPE("GHTextureLoaderDX11::loadMemory", GHString::CHT_REFERENCE);
	if (!extraData)
	{
		GHDebugMessage::outputString("GHTextureLoaderDX11::loadMemory called with no property container, returning 0");
		return 0;
	}

	GHTextureFormat::Enum textureFormat = extraData->getProperty(GHRenderProperties::GP_TEXTUREFORMAT);
	uint32_t width = extraData->getProperty(GHRenderProperties::GP_WIDTH);
	uint32_t height = extraData->getProperty(GHRenderProperties::GP_HEIGHT);
	uint32_t numMips = extraData->getProperty(GHRenderProperties::GP_NUMMIPS);
	bool dontUseMipmaps = extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS);

	bool keepTextureData = extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);
	const char* filename = 0;

	if (textureFormat == GHTextureFormat::TF_RGB8)
	{
		// dx11 does not support rgb8 textures, we need to create a rgba8 buffer.
		return createRGBAFromRGB(mem, memSize, *extraData);
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dtex = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;
	
	DXGI_FORMAT dxFormat = (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureFormat);
	// depth hack.  we say the depth is 4 for rgba8.  the dxt formats ignore this value, and no other formats exist.
	unsigned int depth = 4;

	// create the d3dtex and the textureView
	bool created = mTextureCreator.createTextureFromMemory(d3dtex, textureView, mem, width, height, depth, !dontUseMipmaps, dxFormat, !keepTextureData);
	if (created)
	{
		GHTextureDX11* ret = createGHTexture(d3dtex, textureView, dxFormat, filename, mem, width, height, depth, extraData);
		return ret;
	}

	return 0;
}

// turn a rgb8 texture buffer into a rgba8 texture buffer and pass it on to loadMemory()
GHResource* GHTextureLoaderDX11::createRGBAFromRGB(void* mem, size_t memSize, GHPropertyContainer& extraData)
{
	GHTextureFormat::Enum textureFormat = extraData.getProperty(GHRenderProperties::GP_TEXTUREFORMAT);
	uint32_t width = extraData.getProperty(GHRenderProperties::GP_WIDTH);
	uint32_t height = extraData.getProperty(GHRenderProperties::GP_HEIGHT);
	bool keepTextureData = extraData.getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);
	assert(textureFormat == GHTextureFormat::TF_RGB8);
	extraData.setProperty(GHRenderProperties::GP_TEXTUREFORMAT, GHTextureFormat::TF_RGBA8);

	char* newMem = new char[width*height*4];
	char* newBuff = newMem;
	char* srcBuff = (char*)mem;
	for (size_t pixelId = 0; pixelId < width*height; ++pixelId)
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
	return loadMemory(newMem, width*height*4, &extraData);
}
