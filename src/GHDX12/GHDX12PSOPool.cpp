#include "GHDX12PSOPool.h"
#include "GHVBBlitterIndexDX12.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHMDesc.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHDX12RTGroup.h"

GHDX12PSOPool::GHDX12PSOPool(GHRenderDeviceDX12& device)
	: mDevice(device)
{
}

static D3D12_RASTERIZER_DESC createRasterizerDesc(const GHMDesc& desc)
{
	D3D12_RASTERIZER_DESC ret;

	if (desc.mWireframe)
	{
		ret.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	else
	{
		ret.FillMode = D3D12_FILL_MODE_SOLID;
	}

	if (desc.mCullMode == GHMDesc::CM_NOCULL)
	{
		ret.CullMode = D3D12_CULL_MODE_NONE;
	}
	else if (desc.mCullMode == GHMDesc::CM_CCW)
	{
		ret.CullMode = D3D12_CULL_MODE_FRONT;
	}
	else
	{
		ret.CullMode = D3D12_CULL_MODE_BACK;
	}

	ret.FrontCounterClockwise = TRUE;
	ret.DepthBias = desc.mZOffset;
	ret.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	ret.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	ret.DepthClipEnable = desc.mZRead;
	ret.MultisampleEnable = false; // todo
	ret.AntialiasedLineEnable = false; // todo
	ret.ForcedSampleCount = 0;
	ret.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return ret;
}


static D3D12_BLEND convertGHMBlendToD3D(GHMDesc::BlendMode mode)
{
	if (mode == GHMDesc::BM_ALPHA) return D3D12_BLEND_SRC_ALPHA;
	if (mode == GHMDesc::BM_INVALPHA) return D3D12_BLEND_INV_SRC_ALPHA;
	return D3D12_BLEND_ONE;
}

static D3D12_BLEND_DESC createBlendDesc(const GHMDesc& desc)
{
	D3D12_BLEND_DESC ret;

	ret.AlphaToCoverageEnable = false;
	ret.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC targetBlendDesc = {};
	targetBlendDesc.BlendEnable = desc.mAlphaBlend;
	targetBlendDesc.LogicOpEnable = false;

	if (desc.mAlphaBlend)
	{
		targetBlendDesc.SrcBlend = convertGHMBlendToD3D(desc.mSrcBlend);
		targetBlendDesc.DestBlend = convertGHMBlendToD3D(desc.mDstBlend);
		targetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	}
	else
	{
		targetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		targetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		targetBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	}
	targetBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	targetBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	targetBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	targetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	for (unsigned int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		ret.RenderTarget[i] = targetBlendDesc;
	}

	return ret;
}

static D3D12_DEPTH_STENCIL_DESC createDepthStencilDesc(const GHMDesc& desc)
{
	D3D12_DEPTH_STENCIL_DESC ret;

	if (!desc.mZRead && !desc.mZWrite) {
		ret.DepthEnable = false;
	}
	else {
		ret.DepthEnable = true;
	}

	if (!desc.mZWrite) {
		ret.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	}
	else {
		ret.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	}

	if (!desc.mZRead) {
		ret.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}
	else {
		ret.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	}

	// Stencil test parameters
	ret.StencilEnable = false;
	ret.StencilReadMask = 0xFF;
	ret.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	ret.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ret.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	ret.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	ret.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// Stencil operations if pixel is back-facing
	ret.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	ret.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	ret.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	ret.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return ret;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> GHDX12PSOPool::getPSO(size_t matHash, const GHMDesc& desc, 
	const GHShaderDX12& vs, const GHShaderDX12& ps,
	const GHVertexBuffer& vb, const GHDX12RTGroup& rtGroup)
{
	const GHVBBlitterPtr* vbBlitterPtr = vb.getBlitter();
	GHVBBlitterIndexDX12* ibBlitter = (GHVBBlitterIndexDX12*)(vbBlitterPtr->get());
	D3D12_INPUT_ELEMENT_DESC* ied = ibBlitter->getInputElementDescriptor();
	unsigned int iedCount = ibBlitter->getInputElementCount();

	PoolKey searchKey;
	searchKey.mMatHash = matHash;
	searchKey.mRt0Format = rtGroup.mRt0Format;
	searchKey.mDepthFormat = rtGroup.mDepthFormat;
	searchKey.mIedHash = ibBlitter->getInputElementHash();
	searchKey.mSampleCount = rtGroup.mSampleCount;

	auto findIter = mCache.find(searchKey);
	if (findIter != mCache.end())
	{
		return findIter->second;
	}

	D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
	layoutDesc.NumElements = iedCount;
	layoutDesc.pInputElementDescs = ied;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = layoutDesc;
	psoDesc.pRootSignature = mDevice.getGraphicsRootSignature().Get();
	psoDesc.VS = vs.getBytecode();
	psoDesc.PS = ps.getBytecode();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// todo: support render targets of different formats.
	psoDesc.RTVFormats[0] = rtGroup.mRt0Format;
	psoDesc.DSVFormat = rtGroup.mDepthFormat;
	psoDesc.SampleDesc.Count = rtGroup.mSampleCount;
	psoDesc.SampleDesc.Quality = rtGroup.mSampleQuality;
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = createRasterizerDesc(desc);
	psoDesc.BlendState = createBlendDesc(desc);
	psoDesc.NumRenderTargets = 1;
	psoDesc.DepthStencilState = createDepthStencilDesc(desc);

	Microsoft::WRL::ComPtr<ID3D12PipelineState> ret;
	mDevice.getDXDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&ret));
	mCache[searchKey] = ret;

	GHDebugMessage::outputString("new pso %d", mCache.size());

	return ret;
}

bool GHDX12PSOPool::PoolKey::operator<(const PoolKey& other) const
{
	if (mMatHash != other.mMatHash) return mMatHash < other.mMatHash;
	if (mIedHash != other.mIedHash) return mIedHash < other.mIedHash;
	if (mRt0Format != other.mRt0Format) return mRt0Format < other.mRt0Format;
	if (mDepthFormat != other.mDepthFormat) return mDepthFormat < other.mDepthFormat;
	if (mSampleCount != other.mSampleCount) return mSampleCount < other.mSampleCount;
	return false;
}
