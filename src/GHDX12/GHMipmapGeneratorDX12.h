#pragma once

#include "GHDX12Include.h"
#include "GHShaderDX12.h"
#include "GHDX12CBuffer.h"

class GHResourceFactory;
class GHRenderDeviceDX12;
class GHDX12MaterialHeapPool;

// This is the first compute shader in the engine.
// It is expected that we'll create more shared code the next time we add one.
class GHMipmapGeneratorDX12
{
public:
	GHMipmapGeneratorDX12(GHResourceFactory& resourceFactory, GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& cbufferPool);
	~GHMipmapGeneratorDX12(void);

	void generateMipmaps(Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, DXGI_FORMAT dxFormat, uint32_t width, uint32_t height);

private:
	void createGraphicsRootSignature(void);

private:
	GHRenderDeviceDX12& mDevice;
	GHShaderResource* mShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPipelineState{ nullptr };

	struct CBufferArgs
	{
		uint32_t mSrcMipLevel;		// Texture level of source mip
		uint32_t mNumMipLevels;		// Number of OutMips to write: [1, 4]
		float mTexelSize[2];		// 1.0 / OutMip1.Dimensions
	};
	GHDX12CBuffer mCBuffer;
};
