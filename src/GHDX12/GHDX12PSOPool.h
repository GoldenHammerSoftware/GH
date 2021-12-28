#pragma once

#include "GHDX12Include.h"
#include "GHShaderDX12.h"
#include "Render/GHVertexBuffer.h"

class GHRenderDeviceDX12;
class GHMDesc;

class GHDX12PSOPool
{
public:
	GHDX12PSOPool(GHRenderDeviceDX12& device);

	Microsoft::WRL::ComPtr<ID3D12PipelineState> getPSO(size_t matHash, const GHMDesc& desc, const GHShaderDX12& vs, const GHShaderDX12& ps, const GHVertexBuffer& vb, const DXGI_FORMAT rt0Format, const DXGI_FORMAT depthFormat);

private:
	GHRenderDeviceDX12& mDevice;

	// probable todo: replace this with a hash value.
	struct PoolKey
	{
		size_t mMatHash;
		size_t mIedHash;
		DXGI_FORMAT mRt0Format;
		DXGI_FORMAT mDepthFormat;

		bool operator<(const PoolKey& other) const;
	};
	std::map<PoolKey, Microsoft::WRL::ComPtr<ID3D12PipelineState> > mCache;
};