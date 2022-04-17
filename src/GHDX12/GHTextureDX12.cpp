#include "GHTextureDX12.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHTextureData.h"
#include "Render/GHDXGIUtil.h"
#include "GHPlatform/GHDebugMessage.h"
#include "Render/GHTextureTypeUtil.h"
#include "GHDX12Helpers.h"
#include "GHMipmapGeneratorDX12.h"

static Microsoft::WRL::ComPtr<ID3D12Resource> createDXTexture(GHRenderDeviceDX12& device, GHMipmapGeneratorDX12* mipGen, const GHTextureData& textureData, bool& mipmap)
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
	}
	else
	{
		if (!mipGen)
		{
			mipmap = false;
		}
		resourceDesc.MipLevels = mipmap ? 0 : 1;
	}
	resourceDesc.Format = (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureData.mTextureFormat);
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_HEAP_PROPERTIES destHeapProps;
	GHDX12Helpers::createHeapProperties(destHeapProps, D3D12_HEAP_TYPE_DEFAULT);
	Microsoft::WRL::ComPtr<ID3D12Resource> destDXBuffer;
	HRESULT destRes = device.getDXDevice()->CreateCommittedResource(&destHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(destDXBuffer.GetAddressOf()));
	if (FAILED(destRes))
	{
		GHDebugMessage::outputString("Failed to create texture dest heap");
		return 0;
	}
	destDXBuffer->SetName(L"Texture resource heap");

	// Initialize the upload buffer.
	UINT64 uploadBufferSize;
	device.getDXDevice()->GetCopyableFootprints(&resourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferSize);
	D3D12_HEAP_PROPERTIES uploadHeapProps;
	GHDX12Helpers::createHeapProperties(uploadHeapProps, D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC uploadBufferDesc;
	GHDX12Helpers::createBufferDesc(uploadBufferDesc, (uint32_t)uploadBufferSize);
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadDXBuffer;
	HRESULT copyRes = device.getDXDevice()->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadDXBuffer.GetAddressOf()));
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
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = device.beginUploadCommandList();
			GHDX12Helpers::UpdateSubresources(commandList.Get(), destDXBuffer.Get(), uploadDXBuffer.Get(), 0, 0, 1, &subData);
			D3D12_RESOURCE_BARRIER barrier;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = destDXBuffer.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			commandList->ResourceBarrier(1, &barrier);
			device.endUploadCommandList();
		}
	}

	if (mipmap && textureData.mMipLevels.size() < 2)
	{
		mipGen->generateMipmaps(destDXBuffer, (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(textureData.mTextureFormat), textureData.mMipLevels[0].mWidth, textureData.mMipLevels[0].mHeight);
	}

	return destDXBuffer;
}

GHTextureDX12::GHTextureDX12(GHRenderDeviceDX12& device, GHTextureData* texData, bool mipmap, GHMipmapGeneratorDX12* mipGen)
	: mDevice(device)
	, mTexData(texData)
	, mMipmap(mipmap)
{
	if (!mTexData)
	{
		GHDebugMessage::outputString("Non null texData not allowed when dxBuffer does not exist");
		return;
	}
	mDXFormat = (DXGI_FORMAT)GHDXGIUtil::convertGHFormatToDXGI(texData->mTextureFormat);
	mDXBuffer = createDXTexture(mDevice, mipGen, *mTexData, mMipmap);
}

GHTextureDX12::GHTextureDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, DXGI_FORMAT dxFormat, bool mipmap)
	: mDXBuffer(dxBuffer)
	, mTexData(nullptr)
	, mDevice(device)
	, mDXFormat(dxFormat)
	, mMipmap(mipmap)
{
	if (!mDXBuffer)
	{
		GHDebugMessage::outputString("Non null texData not allowed when dxBuffer does not exist");
	}
}

GHTextureDX12::~GHTextureDX12(void)
{
	deleteSourceData();
}

void GHTextureDX12::deleteSourceData(void)
{
	if (mTexData)
	{
		delete mTexData;
		mTexData = 0;
	}
}

void GHTextureDX12::bind(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int heapTextureStart, unsigned int index)
{
	const UINT cbvSrvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE heapOffsetHandle = heap->GetCPUDescriptorHandleForHeapStart();
	heapOffsetHandle.ptr += ((heapTextureStart+index) * cbvSrvDescriptorSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mDXFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = mMipmap ? -1 : 1;
	mDevice.getDXDevice()->CreateShaderResourceView(mDXBuffer.Get(), &srvDesc, heapOffsetHandle);
}

void GHTextureDX12::createSampler(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int index, GHMDesc::WrapMode wrapMode)
{
	D3D12_CPU_DESCRIPTOR_HANDLE heapOffsetHandle = heap->GetCPUDescriptorHandleForHeapStart();
	const UINT samplerDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	heapOffsetHandle.ptr += (index * samplerDescriptorSize);

	D3D12_TEXTURE_ADDRESS_MODE d3dWrapMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	if (wrapMode == GHMDesc::WM_CLAMP) d3dWrapMode = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	D3D12_SAMPLER_DESC samplerDesc;
	if (mMipmap)
	{
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy = 8;
	}
	else
	{
		samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.MaxAnisotropy = 0;
	}
	samplerDesc.AddressU = d3dWrapMode;
	samplerDesc.AddressV = d3dWrapMode;
	samplerDesc.AddressW = d3dWrapMode;
	samplerDesc.MipLODBias = 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	//samplerDesc.ShaderRegister = index;
	//samplerDesc.RegisterSpace = 0;
	//samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	mDevice.getDXDevice()->CreateSampler(&samplerDesc, heapOffsetHandle);
}

bool GHTextureDX12::lockSurface(void** ret, unsigned int& pitch)
{
	if (!mTexData)
	{
		return false;
	}
	if (GHTextureTypeUtil::isCompressedType(mTexData->mTextureFormat))
	{
		GHDebugMessage::outputString("Tried to lock a compressed texture");
		return false;
	}
	if (!mTexData->mMipLevels.size())
	{
		GHDebugMessage::outputString("Tried to lock a tex data with no mip data");
		return false;
	}
	*ret = mTexData->mMipLevels[0].mData;
	pitch = mTexData->mMipLevels[0].mWidth * mTexData->mDepth;
	return true;
}

bool GHTextureDX12::unlockSurface(void)
{ 
	// nothing to do here.
	return true; 
}

bool GHTextureDX12::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{ 
	if (!mTexData)
	{
		return false;
	}
	if (!mTexData->mMipLevels.size())
	{
		return false;
	}
	width = mTexData->mMipLevels[0].mWidth;
	height = mTexData->mMipLevels[0].mHeight;
	depth = mTexData->mDepth;
	return true;
}
