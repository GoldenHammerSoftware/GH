#pragma once

#include "GHDX12Include.h"
#include "GHShaderDX12.h"
#include "GHDX12CBuffer.h"

class GHResourceFactory;
class GHRenderDeviceDX12;
class GHDX12MaterialHeapPool;

class GHMipmapGeneratorDX12
{
public:
	GHMipmapGeneratorDX12(GHResourceFactory& resourceFactory, GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& cbufferPool);
	~GHMipmapGeneratorDX12(void);

	void generateMipmaps(Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, uint32_t width, uint32_t height);

private:
	void createGraphicsRootSignature(void);

private:
	GHRenderDeviceDX12& mDevice;
	GHShaderResource* mShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mGraphicsRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState{ nullptr };

	struct CBufferArgs
	{
		uint32_t mSrcMipLevel;		// Texture level of source mip
		uint32_t mNumMipLevels;		// Number of OutMips to write: [1, 4]
		float mTexelSize[2];		// 1.0 / OutMip1.Dimensions
	};
	GHDX12CBuffer mCBuffer;
};
