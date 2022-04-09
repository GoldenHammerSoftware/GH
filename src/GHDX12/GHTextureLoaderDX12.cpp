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
	Microsoft::WRL::ComPtr<ID3D12Resource> dxTex = createDXTexture(*textureData, allowMipmaps);
	GHResource* ret = new GHTextureDX12(mDevice, dxTex, 0, dxFormat, allowMipmaps);

	if (!keepTextureData)
	{
		delete textureData;
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
	Microsoft::WRL::ComPtr<ID3D12Resource> dxTex = createDXTexture(*textureData, allowMipmaps);
	GHResource* ret = new GHTextureDX12(mDevice, dxTex, 0, dxFormat, allowMipmaps);

	if (!keepTextureData)
	{
		delete textureData;
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

Microsoft::WRL::ComPtr<ID3D12Resource> GHTextureLoaderDX12::createDXTexture(const GHTextureData& textureData, bool generateMipmaps)
{
	if (!textureData.mMipLevels.size())
	{
		GHDebugMessage::outputString("No mip levels on texture data");
		return nullptr;
	}
	// initialize the destination buffer.
	D3D12_RESOURCE_DESC resourceDesc;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = textureData.mMipLevels[0].mWidth;
	resourceDesc.Height = textureData.mMipLevels[0].mHeight;
	resourceDesc.DepthOrArraySize = textureData.mNumSlices;
	if (textureData.mMipLevels.size() > 1)
	{
		resourceDesc.MipLevels = textureData.mMipLevels.size();
		generateMipmaps = false;
	}
	else
	{
		resourceDesc.MipLevels = generateMipmaps ? 0 : 1;
	}
	resourceDesc.Format = (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureData.mTextureFormat);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES destHeapProps;
	GHDX12Helpers::createHeapProperties(destHeapProps, D3D12_HEAP_TYPE_DEFAULT);
	Microsoft::WRL::ComPtr<ID3D12Resource> destDXBuffer;
	HRESULT destRes = mDevice.getDXDevice()->CreateCommittedResource(&destHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(destDXBuffer.GetAddressOf()));
	if (FAILED(destRes))
	{
		GHDebugMessage::outputString("Failed to create texture dest heap");
		return 0;
	}
	destDXBuffer->SetName(L"Texture resource heap");

	// Initialize the upload buffer.
	UINT64 uploadBufferSize;
	mDevice.getDXDevice()->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
	D3D12_HEAP_PROPERTIES uploadHeapProps;
	GHDX12Helpers::createHeapProperties(uploadHeapProps, D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC uploadBufferDesc;
	GHDX12Helpers::createBufferDesc(uploadBufferDesc, (uint32_t)uploadBufferSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadDXBuffer;
	HRESULT copyRes = mDevice.getDXDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadDXBuffer.GetAddressOf()));
	if (FAILED(copyRes))
	{
		GHDebugMessage::outputString("Failed to create dx upload texture heap");
		return 0;
	}
	uploadDXBuffer->SetName(L"Upload texture heap");

	// todo: slices and mipmaps
	//for (int slice = 0; slice < textureData.mNumSlices; ++slice)
	{
		//for (int mip = 0; mip < textureData.mMipLevels.size(); ++mip)
		{
			int imageBytesPerRow = textureData.mMipLevels[0].mWidth * textureData.mDepth;
			D3D12_SUBRESOURCE_DATA subData = {};
			subData.pData = textureData.mMipLevels[0].mData;
			subData.RowPitch = imageBytesPerRow;
			subData.SlicePitch = imageBytesPerRow * textureData.mMipLevels[0].mHeight;

			// Copy into the destination buffer.
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = mDevice.beginUploadCommandList();
			GHDX12Helpers::UpdateSubresources(commandList.Get(), destDXBuffer.Get(), uploadDXBuffer.Get(), 0, 0, 1, &subData);
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = destDXBuffer.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			commandList->ResourceBarrier(1, &barrier);
			mDevice.endUploadCommandList();
		}
	}

	if (generateMipmaps)
	{
		mMipGen.generateMipmaps(destDXBuffer, (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureData.mTextureFormat), textureData.mMipLevels[0].mWidth, textureData.mMipLevels[0].mHeight);
	}

	return destDXBuffer;
}

void GHTextureLoaderDX12::addOverrideLoader(GHResourceLoader* loader)
{
	mOverrideLoaders.push_back(loader);
}
