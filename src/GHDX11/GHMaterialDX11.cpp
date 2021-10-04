// Copyright Golden Hammer Software
#include "GHMaterialDX11.h"
#include "GHBillboardTransformAdjuster.h"
#include "GHMath/GHTransform.h"
#include "GHShaderParamListDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX11.h"
#include "GHTexture.h"
#include "GHTextureBindArgsDX11.h"
#include "GHTextureDX11.h"
#include "GHMaterialParamHandleDX11.h"
#include "GHMaterialCallback.h"
#include "GHRenderStateMgrDX11.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"

GHMaterialDX11::GHMaterialDX11(GHRenderDeviceDX11& device, 
							   GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps,
							   GHRenderStateMgrDX11& stateMgr, GHEventMgr& eventMgr, bool isOverrideMat)
: mDevice(device)
, mDesc(desc)
, mBlendState(0)
, mDepthStencilState(0)
, mRasterizerState(0)
, mStateMgr(stateMgr)
, mEventMgr(eventMgr)
, mMessageListener(*this)
{
	mShaders[GHShaderType::ST_VERTEX] = new GHMaterialShaderInfoDX11(device, vs);
	mShaders[GHShaderType::ST_PIXEL] = new GHMaterialShaderInfoDX11(device, ps);

	GHMDesc::ParamHandles* descParamHandles = desc->initMaterial(*this);
	desc->applyDefaultArgs(*this, *descParamHandles);
	desc->applyTextures(*this, *descParamHandles, isOverrideMat);
	delete descParamHandles;

	createRasterizerState();
	createDepthStencilState();
	createBlendState();

	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

GHMaterialDX11::~GHMaterialDX11(void)
{
	delete mDesc;
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		delete mShaders[shaderType];
	}
	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

void GHMaterialDX11::beginMaterial(const GHViewInfo&)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		mStateMgr.applyShader(mShaders[shaderType]->mShader, (GHShaderType::Enum)shaderType);
	}

	mStateMgr.applyRasterizerState(mRasterizerState);
	mStateMgr.applyDepthStencilState(mDepthStencilState);
	mStateMgr.applyBlendState(mBlendState);

	applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
	sendVSArgs(GHMaterialCallbackType::CT_PERFRAME);
	sendPSArgs(GHMaterialCallbackType::CT_PERFRAME);
}

void GHMaterialDX11::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(geoData, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbArg);
	sendVSArgs(GHMaterialCallbackType::CT_PERGEO);
	sendPSArgs(GHMaterialCallbackType::CT_PERGEO);
}

void GHMaterialDX11::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(entData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbArg);
	sendVSArgs(GHMaterialCallbackType::CT_PERENT);
	sendPSArgs(GHMaterialCallbackType::CT_PERENT);
}

void GHMaterialDX11::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
    GHBillboardTransformAdjuster transAdjuster;
    GHTransform scratchTransform;
    const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);
    
    GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
	sendVSArgs(GHMaterialCallbackType::CT_PERTRANS);
	sendPSArgs(GHMaterialCallbackType::CT_PERTRANS);
}

void GHMaterialDX11::endMaterial(void)
{
	ID3D11ShaderResourceView* nullRes[8] = {0,0,0,0,0,0,0,0};
	ID3D11SamplerState* nullSampler[8] = {0,0,0,0,0,0,0,0};
	mDevice.getD3DContext()->PSSetShaderResources(0, 8, &nullRes[0]);
	mDevice.getD3DContext()->PSSetSamplers(0, 8, &nullSampler[0]);

	// textures are not allowed in vs on phone, this crashes if called there.
	// todo: make this a dx11 only feature somehow.
//	mDevice.getD3DContext()->VSSetShaderResources(0, 8, &nullRes[0]);
//	mDevice.getD3DContext()->VSSetSamplers(0, 8, &nullSampler[0]);
}

GHMaterialParamHandle* GHMaterialDX11::getParamHandle(const char* paramName)
{
	GHMaterialParamHandleDX11* ret = 0;
	GHString paramString(mDesc->checkParamAlias(paramName), GHString::CHT_REFERENCE);
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) 
	{
		const GHShaderParamListDX11& paramList = mShaders[shaderType]->mShader->get()->getParamList();
		const GHShaderParamListDX11::Param* param = paramList.getParam(paramString);
		if (param)
		{
			if (!ret) ret = new GHMaterialParamHandleDX11;
			ret->addShaderHandle(mShaders[shaderType], param);
		}
	}
	return ret;
}

void GHMaterialDX11::sendVSArgs(GHMaterialCallbackType::Enum ct)
{
	if (mShaders[GHShaderType::ST_VERTEX]->mCBuffers[(int)ct] && 
		mShaders[GHShaderType::ST_VERTEX]->mCBuffers[(int)ct]->mD3DBuffer) 
	{
		mShaders[GHShaderType::ST_VERTEX]->mCBuffers[(int)ct]->updateD3DBuffer(mDevice);
		mDevice.getD3DContext()->VSSetConstantBuffers((int)ct, 1, 
			mShaders[GHShaderType::ST_VERTEX]->mCBuffers[(int)ct]->mD3DBuffer.GetAddressOf());
	}
	for (unsigned int i = 0; i < mShaders[GHShaderType::ST_VERTEX]->mTextures[(int)ct].size(); ++i)
	{
		GHMaterialShaderInfoDX11::TextureSlot* currTex = mShaders[GHShaderType::ST_VERTEX]->mTextures[(int)ct][i];
		if (!currTex->getTexture()) continue;

		GHTextureDX11* currTexDX = (GHTextureDX11*)currTex->getTexture();
		GHTextureBindArgsDX11 bindArgs;
		bindArgs.mIsVertex = true;
		bindArgs.mRegister = currTex->getRegister();
		currTexDX->setBindArgs(bindArgs);

		currTex->getTexture()->bind();

		mDevice.getD3DContext()->VSSetSamplers(currTex->getRegister(), 1, currTex->getSampler().GetAddressOf());
	}
}

void GHMaterialDX11::sendPSArgs(GHMaterialCallbackType::Enum ct)
{
	if (mShaders[GHShaderType::ST_PIXEL]->mCBuffers[(int)ct] && 
		mShaders[GHShaderType::ST_PIXEL]->mCBuffers[(int)ct]->mD3DBuffer) 
	{
		mShaders[GHShaderType::ST_PIXEL]->mCBuffers[(int)ct]->updateD3DBuffer(mDevice);
		mDevice.getD3DContext()->PSSetConstantBuffers((int)ct, 1, 
			mShaders[GHShaderType::ST_PIXEL]->mCBuffers[(int)ct]->mD3DBuffer.GetAddressOf());
	}
	for (unsigned int i = 0; i < mShaders[GHShaderType::ST_PIXEL]->mTextures[(int)ct].size(); ++i)
	{
		GHMaterialShaderInfoDX11::TextureSlot* currTex = mShaders[GHShaderType::ST_PIXEL]->mTextures[(int)ct][i];
		if (!currTex->getTexture()) continue;

		GHTextureDX11* currTexDX = (GHTextureDX11*)currTex->getTexture();
		GHTextureBindArgsDX11 bindArgs;
		bindArgs.mIsVertex = false;
		bindArgs.mRegister = currTex->getRegister();
		currTexDX->setBindArgs(bindArgs);

		currTex->getTexture()->bind();

		mDevice.getD3DContext()->PSSetSamplers(currTex->getRegister(), 1, currTex->getSampler().GetAddressOf());
	}
}

void GHMaterialDX11::createRasterizerState(void)
{
	if (mDesc->mWireframe == false) {
		mRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	}
	else {
		mRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	}

	if (mDesc->mCullMode == GHMDesc::CM_NOCULL) {
		mRasterizerDesc.CullMode = D3D11_CULL_NONE;
	}
	else if (mDesc->mCullMode == GHMDesc::CM_CW) {
		mRasterizerDesc.CullMode = D3D11_CULL_BACK;
	}
	else {
		mRasterizerDesc.CullMode = D3D11_CULL_FRONT;
	}
	mRasterizerDesc.FrontCounterClockwise = TRUE;
	mRasterizerDesc.DepthBias = 0;
	mRasterizerDesc.DepthBiasClamp = 0;
	mRasterizerDesc.SlopeScaledDepthBias = 0;
	mRasterizerDesc.DepthClipEnable = TRUE;
	mRasterizerDesc.ScissorEnable = FALSE;
	mRasterizerDesc.MultisampleEnable = FALSE;
	mRasterizerDesc.AntialiasedLineEnable = FALSE;

	mDevice.getD3DDevice()->CreateRasterizerState(&mRasterizerDesc, mRasterizerState.GetAddressOf());
}

void GHMaterialDX11::createDepthStencilState(void)
{
	// Depth test parameters
	if (!mDesc->mZRead && !mDesc->mZWrite) {
		mDepthStencilDesc.DepthEnable = false;
	}
	else {
		mDepthStencilDesc.DepthEnable = true;
	}

	if (!mDesc->mZWrite) {
		mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	}
	else {
		mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	}

	if (!mDesc->mZRead) {
		mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	}
	else {
		mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	}

	// Stencil test parameters
	mDepthStencilDesc.StencilEnable = false;
	mDepthStencilDesc.StencilReadMask = 0xFF;
	mDepthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	mDevice.getD3DDevice()->CreateDepthStencilState(&mDepthStencilDesc, mDepthStencilState.GetAddressOf());
}

static D3D11_BLEND convertGHMBlendToD3D(GHMDesc::BlendMode mode)
{
	if (mode == GHMDesc::BM_ALPHA) return D3D11_BLEND_SRC_ALPHA;
	if (mode == GHMDesc::BM_INVALPHA) return D3D11_BLEND_INV_SRC_ALPHA;
	return D3D11_BLEND_ONE;
}

void GHMaterialDX11::createBlendState(void)
{
	mBlendDesc.AlphaToCoverageEnable = FALSE;
	mBlendDesc.IndependentBlendEnable = FALSE;
	if (mDesc->mAlphaBlend)
	{
		mIsAlpha = true;
		mBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		mBlendDesc.RenderTarget[0].SrcBlend = convertGHMBlendToD3D(mDesc->mSrcBlend);
		mBlendDesc.RenderTarget[0].DestBlend = convertGHMBlendToD3D(mDesc->mDstBlend);
		mBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	}
	else
	{
		mIsAlpha = false;
		mBlendDesc.RenderTarget[0].BlendEnable = FALSE;
		mBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		mBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		mBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	}

	mBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	mBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	mBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	mBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT res = mDevice.getD3DDevice()->CreateBlendState(&mBlendDesc, mBlendState.GetAddressOf());
	if (FAILED(res)) {
		GHDebugMessage::outputString("Failed to create blend state.");
	}
}

void GHMaterialDX11::reinit(void)
{
	mRasterizerState = nullptr;
	mDepthStencilState = nullptr;
	mBlendState = nullptr;

	createRasterizerState();
	createDepthStencilState();
	createBlendState();

	mShaders[GHShaderType::ST_VERTEX]->reinit();
	mShaders[GHShaderType::ST_PIXEL]->reinit();
}
