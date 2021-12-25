#include "GHTextureDX12.h"
#include "GHRenderDeviceDX12.h"

GHTextureDX12::GHTextureDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, void* mem, DXGI_FORMAT dxFormat, bool mipmap)
	: mDXBuffer(dxBuffer)
	, mMem(mem)
	, mDevice(device)
	, mDXFormat(dxFormat)
	, mMipmap(mipmap)
{
}

GHTextureDX12::~GHTextureDX12(void)
{
	deleteSourceData();
}

void GHTextureDX12::deleteSourceData(void)
{
	if (mMem)
	{
		delete mMem;
		mMem = 0;
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
