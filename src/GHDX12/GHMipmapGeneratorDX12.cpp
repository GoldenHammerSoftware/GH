#include "GHMipmapGeneratorDX12.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"

GHMipmapGeneratorDX12::GHMipmapGeneratorDX12(GHResourceFactory& resourceFactory, GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& cbufferPool)
	: mDevice(device)
	, mCBuffer(device, cbufferPool, sizeof(CBufferArgs))
{
	mShader = (GHShaderResource*)resourceFactory.getCacheResource("generatemipscomputeshader.hlsl");
	if (mShader) mShader->acquire();
	else GHDebugMessage::outputString("Failed to load mipmap compute shader");

	createGraphicsRootSignature();

	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = mRootSignature.Get();
	psoDesc.CS = mShader->get()->getBytecode();
	mDevice.getDXDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(mPipelineState.GetAddressOf()));
}

GHMipmapGeneratorDX12::~GHMipmapGeneratorDX12(void)
{
	if (mShader) mShader->release();
}

static void incrementMipSize(uint32_t& width, uint32_t& height)
{
	if (width > 1) width /= 2;
	if (width < 1) width = 1;
	if (height > 1) height /= 2;
	if (height < 1) height = 1;
}

void GHMipmapGeneratorDX12::generateMipmaps(Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, DXGI_FORMAT dxFormat, uint32_t width, uint32_t height)
{
	if (!mShader) return;
	if (!mPipelineState) return;

	// calc num mips, probably move this to the caller.
	uint32_t testWidth = width;
	uint32_t testHeight = height;
	int numMips = 0;
	while (true)
	{
		numMips++;
		if (testWidth == 1 && testHeight == 1)
		{
			break;
		}
		incrementMipSize(testWidth, testHeight);
	}
	if (numMips == 1) return;

	// create the srv/uav descriptor heap.
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	int numDrawPasses = max(1, ceil((float)numMips / 4.0f));
	heapDesc.NumDescriptors = 2 * numDrawPasses + 4 * numDrawPasses;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	mDevice.getDXDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	UINT descriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_GPU_DESCRIPTOR_HANDLE heapGPUOffsetHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE heapCPUOffsetHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srcTextureSRVDesc = {};
	srcTextureSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srcTextureSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srcTextureSRVDesc.Format = dxFormat;

	D3D12_UNORDERED_ACCESS_VIEW_DESC destTextureUAVDesc = {};
	destTextureUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	destTextureUAVDesc.Format = dxFormat;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cl = mDevice.beginComputeCommandList();
	cl->SetComputeRootSignature(mRootSignature.Get());
	cl->SetPipelineState(mPipelineState.Get());
	cl->SetDescriptorHeaps(1, descriptorHeap.GetAddressOf());

	D3D12_RESOURCE_BARRIER toUAVBarrier;
	toUAVBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	toUAVBarrier.Transition.pResource = dxBuffer.Get();
	toUAVBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	toUAVBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	toUAVBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	toUAVBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	cl->ResourceBarrier(1, &toUAVBarrier);

	int currDestMip = 1; // mip 0 is always intitial source.
	uint32_t destWidth = width/2;
	uint32_t destHeight = height/2;
	CBufferArgs* cbufferArgs = (CBufferArgs*)mCBuffer.getMemoryBuffer();
	uint32_t heapIndex = 0;
	while (currDestMip < numMips)
	{
		cbufferArgs->mSrcMipLevel = currDestMip - 1;
		cbufferArgs->mNumMipLevels = 1;
		cbufferArgs->mTexelSize[0] = 1.0f / (float)(destWidth);
		cbufferArgs->mTexelSize[1] = 1.0f / (float)(destHeight);

		for (int i = 0; i < 3; ++i)
		{
			if (destWidth == 1 && destHeight == 1)
			{
				break;
			}
			incrementMipSize(destWidth, destHeight);
			cbufferArgs->mNumMipLevels++;
		}
		mCBuffer.updateFrameData();
		mCBuffer.createSRV(descriptorHeap, heapIndex);
		cl->SetComputeRootDescriptorTable(0, heapGPUOffsetHandle);
		heapGPUOffsetHandle.ptr += descriptorSize;
		heapCPUOffsetHandle.ptr += descriptorSize;

		srcTextureSRVDesc.Texture2D.MipLevels = 1;
		srcTextureSRVDesc.Texture2D.MostDetailedMip = cbufferArgs->mSrcMipLevel;
		mDevice.getDXDevice()->CreateShaderResourceView(dxBuffer.Get(), &srcTextureSRVDesc, heapCPUOffsetHandle);
		cl->SetComputeRootDescriptorTable(1, heapGPUOffsetHandle);
		heapGPUOffsetHandle.ptr += descriptorSize;
		heapCPUOffsetHandle.ptr += descriptorSize;

		D3D12_GPU_DESCRIPTOR_HANDLE uavGPUStart = heapGPUOffsetHandle;
		for (int i = 1; i < 5; ++i)
		{
			if (cbufferArgs->mSrcMipLevel + i < numMips)
			{
				destTextureUAVDesc.Texture2D.MipSlice = cbufferArgs->mSrcMipLevel + i;
				mDevice.getDXDevice()->CreateUnorderedAccessView(dxBuffer.Get(), nullptr, &destTextureUAVDesc, heapCPUOffsetHandle);
				heapGPUOffsetHandle.ptr += descriptorSize;
				heapCPUOffsetHandle.ptr += descriptorSize;
			}
		}
		cl->SetComputeRootDescriptorTable(2, uavGPUStart);
		currDestMip += 4;

		//Dispatch the compute shader with one thread per 8x8 pixels
		cl->Dispatch(max(destWidth / 8, 1u), max(destHeight / 8, 1u), 1);

		// wait for the result before passing the next mip as source.
		D3D12_RESOURCE_BARRIER waitBarrier;
		waitBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
		waitBarrier.UAV.pResource = dxBuffer.Get();
		waitBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		cl->ResourceBarrier(1, &waitBarrier);
	}

	D3D12_RESOURCE_BARRIER fromUAVBarrier;
	fromUAVBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	fromUAVBarrier.Transition.pResource = dxBuffer.Get();
	fromUAVBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	fromUAVBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	fromUAVBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	fromUAVBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	cl->ResourceBarrier(1, &fromUAVBarrier);

	mDevice.endComputeCommandList();
}

void GHMipmapGeneratorDX12::createGraphicsRootSignature(void)
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER params[3];
	desc.NumParameters = 3;
	desc.pParameters = &params[0];

	// cbuffers
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// textures
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	// uav
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// params cbv
	D3D12_DESCRIPTOR_RANGE cbvDescRange;
	cbvDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvDescRange.NumDescriptors = 1;
	cbvDescRange.BaseShaderRegister = 0;
	cbvDescRange.RegisterSpace = 0;
	cbvDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[0].DescriptorTable.NumDescriptorRanges = 1;
	params[0].DescriptorTable.pDescriptorRanges = &cbvDescRange;

	// input texture
	D3D12_DESCRIPTOR_RANGE srvDescRange;
	srvDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescRange.NumDescriptors = 1;
	srvDescRange.BaseShaderRegister = 0;
	srvDescRange.RegisterSpace = 0;
	srvDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &srvDescRange;

	// output uavs
	D3D12_DESCRIPTOR_RANGE uavDescRange;
	uavDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
	uavDescRange.NumDescriptors = 4;
	uavDescRange.BaseShaderRegister = 0;
	uavDescRange.RegisterSpace = 0;
	uavDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[2].DescriptorTable.NumDescriptorRanges = 1;
	params[2].DescriptorTable.pDescriptorRanges = &uavDescRange;

	// static sampler
	desc.NumStaticSamplers = 1;
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.ShaderRegister = 0;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	desc.pStaticSamplers = &samplerDesc;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = 0;
	Microsoft::WRL::ComPtr<ID3DBlob> blobError = 0;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), blobError.GetAddressOf());
	if (FAILED(hr))
	{
		const char* errorBuffer = "";
		if (blobError)
		{
			errorBuffer = (const char*)blobError->GetBufferPointer();
		}
		GHDebugMessage::outputString("Failed to create root signature blob: %s", errorBuffer);
		return;
	}

	auto blobBuffer = signatureBlob->GetBufferPointer();
	auto blobBufferSize = signatureBlob->GetBufferSize();
	hr = mDevice.getDXDevice()->CreateRootSignature(0, blobBuffer, blobBufferSize, IID_PPV_ARGS(&mRootSignature));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create root signature");
		return;
	}
}
