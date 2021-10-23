#include "GHMaterialDX12.h"
#include "Render/GHMDesc.h"
#include "GHMaterialShaderInfoDX12.h"
#include "GHMaterialParamHandleDX12.h"
#include "Render/GHMaterialCallback.h"
#include "Render/GHBillboardTransformAdjuster.h"
#include "GHMath/GHTransform.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHVertexBuffer.h"
#include "GHVBBlitterIndexDX12.h"

GHMaterialDX12::GHMaterialDX12(GHRenderDeviceDX12& device, GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps)
	: mDevice(device)
	, mDesc(desc)
{
	mShaders[GHShaderType::ST_VERTEX] = new GHMaterialShaderInfoDX12(device, vs);
	mShaders[GHShaderType::ST_PIXEL] = new GHMaterialShaderInfoDX12(device, ps);

	GHMDesc::ParamHandles* descParamHandles = desc->initMaterial(*this);
	desc->applyDefaultArgs(*this, *descParamHandles);
	desc->applyTextures(*this, *descParamHandles);
	delete descParamHandles;

	createRasterizerDesc();
	createBlendDesc();

	// todo: GHRenderProperties::DEVICEREINIT
}

GHMaterialDX12::~GHMaterialDX12(void)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		delete mShaders[shaderType];
	}
}

void GHMaterialDX12::beginMaterial(const GHViewInfo& viewInfo)
{
	applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
	applyDXArgs(GHMaterialCallbackType::CT_PERFRAME);
}

void GHMaterialDX12::beginVB(const GHVertexBuffer& vb)
{
	createPSO(vb);
	mDevice.getRenderCommandList()->SetPipelineState(mPSO.Get());
}

void GHMaterialDX12::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(geoData, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERGEO);
}

void GHMaterialDX12::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(entData, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERENT);
}

void GHMaterialDX12::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
	GHBillboardTransformAdjuster transAdjuster;
	GHTransform scratchTransform;
	const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);

	GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERTRANS);
}

void GHMaterialDX12::endMaterial(void)
{
	// reset anything we don't want to carry over to the next draw.
}

GHMaterialParamHandle* GHMaterialDX12::getParamHandle(const char* paramName)
{
	GHMaterialParamHandleDX12* ret = 0;
	GHString paramString(mDesc->checkParamAlias(paramName), GHString::CHT_REFERENCE);
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		const GHShaderParamList& paramList = *mShaders[shaderType]->mShader->get()->getParamList();
		const GHShaderParamList::Param* param = paramList.getParam(paramString);
		if (param)
		{
			if (!ret) ret = new GHMaterialParamHandleDX12;
			ret->addShaderHandle(mShaders[shaderType], param);
		}
	}
	return ret;
}

void GHMaterialDX12::applyDXArgs(GHMaterialCallbackType::Enum type)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		if (!mShaders[shaderType]->mCBuffers[type]) continue;
		mShaders[shaderType]->mCBuffers[type]->updateFrameData(mDevice.getFrameBackendId());
	}
	// todo: add to encoder.
}

void GHMaterialDX12::createPSO(const GHVertexBuffer& vb)
{
	// todo: support multiple PSOs
	if (mPSO)
	{
		return;
	}

	const GHVBBlitterPtr* vbBlitterPtr = vb.getBlitter();
	GHVBBlitterIndexDX12* ibBlitter = (GHVBBlitterIndexDX12*)(vbBlitterPtr->get());
	D3D12_INPUT_ELEMENT_DESC* ied = ibBlitter->getInputElementDescriptor();
	unsigned int iedCount = ibBlitter->getInputElementCount();
	D3D12_INPUT_LAYOUT_DESC layoutDesc = {};
	layoutDesc.NumElements = iedCount;
	layoutDesc.pInputElementDescs = ied;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = layoutDesc;
	psoDesc.pRootSignature = mDevice.getGraphicsRootSignature().Get();
	psoDesc.VS = mShaders[GHShaderType::ST_VERTEX]->mShader->get()->getBytecode();
	psoDesc.PS = mShaders[GHShaderType::ST_PIXEL]->mShader->get()->getBytecode();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// todo: support render targets of different formats.
	psoDesc.RTVFormats[0] = SWAP_BUFFER_FORMAT;
	psoDesc.SampleDesc = mDevice.getSampleDesc();
	psoDesc.SampleMask = 0xffffffff; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	psoDesc.RasterizerState = mRasterizerDesc;
	psoDesc.BlendState = mBlendDesc;
	psoDesc.NumRenderTargets = 1;

	mDevice.getDXDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO));
}

void GHMaterialDX12::createRasterizerDesc(void)
{
	if (mDesc->mWireframe)
	{
		mRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	else
	{
		mRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	}

	if (mDesc->mCullMode == GHMDesc::CM_NOCULL)
	{
		mRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	}
	else if (mDesc->mCullMode == GHMDesc::CM_CCW)
	{
		mRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
	}
	else
	{
		mRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	}
	
	mRasterizerDesc.FrontCounterClockwise = TRUE;
	mRasterizerDesc.DepthBias = mDesc->mZOffset;
	mRasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	mRasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	mRasterizerDesc.DepthClipEnable = mDesc->mZRead;
	mRasterizerDesc.MultisampleEnable = false; // todo
	mRasterizerDesc.AntialiasedLineEnable = false; // todo
	mRasterizerDesc.ForcedSampleCount = 0;
	mRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

static D3D12_BLEND convertGHMBlendToD3D(GHMDesc::BlendMode mode)
{
	if (mode == GHMDesc::BM_ALPHA) return D3D12_BLEND_SRC_ALPHA;
	if (mode == GHMDesc::BM_INVALPHA) return D3D12_BLEND_INV_SRC_ALPHA;
	return D3D12_BLEND_ONE;
}

void GHMaterialDX12::createBlendDesc(void)
{
	mIsAlpha = mDesc->mAlphaBlend;

	mBlendDesc.AlphaToCoverageEnable = false;
	mBlendDesc.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC targetBlendDesc = {};
	targetBlendDesc.BlendEnable = mIsAlpha;
	targetBlendDesc.LogicOpEnable = false;

	if (mDesc->mAlphaBlend)
	{
		targetBlendDesc.SrcBlend = convertGHMBlendToD3D(mDesc->mSrcBlend);
		targetBlendDesc.DestBlend = convertGHMBlendToD3D(mDesc->mDstBlend);
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
		mBlendDesc.RenderTarget[i] = targetBlendDesc;
	}
}
