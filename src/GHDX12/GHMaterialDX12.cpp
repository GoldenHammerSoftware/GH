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
	mShaders[GHShaderType::ST_VERTEX] = new GHMaterialShaderInfoDX12(vs);
	mShaders[GHShaderType::ST_PIXEL] = new GHMaterialShaderInfoDX12(ps);

	GHMDesc::ParamHandles* descParamHandles = desc->initMaterial(*this);
	desc->applyDefaultArgs(*this, *descParamHandles);
	desc->applyTextures(*this, *descParamHandles);
	delete descParamHandles;

	createRootSignature();
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
	mDevice.getRenderCommandList()->SetGraphicsRootSignature(mRootSignature);

	applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
	applyDXArgs(GHMaterialCallbackType::CT_PERFRAME);
}

void GHMaterialDX12::beginVB(const GHVertexBuffer& vb)
{
	createPSO(vb);
	mDevice.getRenderCommandList()->SetPipelineState(mPSO);
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
	// todo.
}

void GHMaterialDX12::createRootSignature(void)
{
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Create two descriptor tables for the cbuffers.  one visible by vertex and one visible by pixel.
	// also one for the pixel srv (add one for vertex?)
	D3D12_ROOT_PARAMETER params[3];
	desc.NumParameters = 3;
	desc.pParameters = &params[0];

	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_DESCRIPTOR_RANGE descRange;
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRange.NumDescriptors = 4;
	descRange.BaseShaderRegister = 0;
	descRange.RegisterSpace = 0;
	descRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[0].DescriptorTable.NumDescriptorRanges = 1;
	params[0].DescriptorTable.pDescriptorRanges = &descRange;
	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &descRange;

	const size_t maxTextures = 16;

	D3D12_DESCRIPTOR_RANGE srvDescRange;
	srvDescRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvDescRange.NumDescriptors = maxTextures;
	srvDescRange.BaseShaderRegister = 0;
	srvDescRange.RegisterSpace = 0;
	srvDescRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	params[2].DescriptorTable.NumDescriptorRanges = 1;
	params[2].DescriptorTable.pDescriptorRanges = &srvDescRange;

	desc.NumStaticSamplers = maxTextures;
	// todo: support different wrap modes etc.
	// we should probably canonize the types of samplers that are supported and use those in the shaders
	//  and remove the option of having the texture specify these things.
	D3D12_STATIC_SAMPLER_DESC samplers[maxTextures];
	for (int i = 0; i < maxTextures; ++i)
	{
		samplers[i].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		samplers[i].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		samplers[i].MipLODBias = 0;
		samplers[i].MaxAnisotropy = 0;
		samplers[i].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		samplers[i].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		samplers[i].MinLOD = 0.0f;
		samplers[i].MaxLOD = D3D12_FLOAT32_MAX;
		samplers[i].ShaderRegister = i;
		samplers[i].RegisterSpace = 0;
		samplers[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}
	desc.pStaticSamplers = &samplers[0];

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
	psoDesc.pRootSignature = mRootSignature;
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
