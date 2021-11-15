#include "GHTextureDX12.h"
#include "GHRenderDeviceDX12.h"

GHTextureDX12::GHTextureDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, void* mem, DXGI_FORMAT dxFormat)
	: mDXBuffer(dxBuffer)
	, mMem(mem)
	, mDevice(device)
	, mDXFormat(dxFormat)
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
	srvDesc.Texture2D.MipLevels = 1;
	mDevice.getDXDevice()->CreateShaderResourceView(mDXBuffer.Get(), &srvDesc, heapOffsetHandle);
}
