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
	psoDesc.pRootSignature = mGraphicsRootSignature.Get();
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

void GHMipmapGeneratorDX12::generateMipmaps(Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, uint32_t width, uint32_t height)
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

	int currDestMip = 1; // mip 0 is always intitial source.
	uint32_t destWidth = width/2;
	uint32_t destHeight = height/2;
	CBufferArgs* cbufferArgs = (CBufferArgs*)mCBuffer.getMemoryBuffer();
	while (currDestMip < numMips)
	{
		cbufferArgs->mSrcMipLevel = currDestMip - 1;
		cbufferArgs->mNumMipLevels = 1;
		cbufferArgs->mTexelSize[0] = 1.0f / (float)(destWidth);
		cbufferArgs->mTexelSize[1] = 1.0f / (float)(destHeight);

		for (int i = 0; i < 3; ++i)
		{
			if (testWidth == 1 && testHeight == 1)
			{
				break;
			}
			incrementMipSize(destWidth, destHeight);
			cbufferArgs->mNumMipLevels++;
		}
		mCBuffer.updateFrameData();

		// and draw.


	}
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
	hr = mDevice.getDXDevice()->CreateRootSignature(0, blobBuffer, blobBufferSize, IID_PPV_ARGS(&mGraphicsRootSignature));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create root signature");
		return;
	}
}
