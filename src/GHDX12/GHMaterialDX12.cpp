#include "GHMaterialDX12.h"
#include "Render/GHMDesc.h"
#include "GHMaterialShaderInfoDX12.h"
#include "GHMaterialParamHandleDX12.h"
#include "Render/GHMaterialCallback.h"
#include "Render/GHBillboardTransformAdjuster.h"
#include "GHMath/GHTransform.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"

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
	// todo: populate with real data.
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = 0;
	desc.pParameters = nullptr;
	desc.NumStaticSamplers = 0;
	desc.pStaticSamplers = nullptr;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// leaking signatureBlob?
	ID3DBlob* signatureBlob;
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, nullptr);
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create root signature blob");
		return;
	}

	hr = mDevice.getDXDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Failed to create root signature");
		return;
	}
}
