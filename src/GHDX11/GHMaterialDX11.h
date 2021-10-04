// Copyright Golden Hammer Software
#pragma once

#include "GHMaterial.h"
#include "GHShaderDX11.h"
#include "GHMDesc.h"
#include "GHMaterialShaderInfoDX11.h"
#include "GHMaterialCallbackType.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHShaderType.h"

class GHRenderDeviceDX11;
class GHRenderStateMgrDX11;
class GHEventMgr;

class GHMaterialDX11 : public GHMaterial
{
public:
	GHMaterialDX11(GHRenderDeviceDX11& device, 
		GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps, 
		GHRenderStateMgrDX11& stateMgr, GHEventMgr& eventMgr,
		bool isOverrideMat);
	~GHMaterialDX11(void);

    virtual void beginMaterial(const GHViewInfo& viewInfo);
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo);
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo);
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo);
    virtual void endMaterial(void);

    virtual GHMaterialParamHandle* getParamHandle(const char* paramName);

	void reinit(void);

private:
	void sendVSArgs(GHMaterialCallbackType::Enum ct);
	void sendPSArgs(GHMaterialCallbackType::Enum ct);
	void createRasterizerState(void);
	void createDepthStencilState(void);
	void createBlendState(void);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHMaterialDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHMaterialDX11& mParent;
	};

private:
	GHRenderDeviceDX11& mDevice;
	GHRenderStateMgrDX11& mStateMgr;
	GHMDesc* mDesc;
	GHEventMgr& mEventMgr;
	MessageListener mMessageListener;

	GHMaterialShaderInfoDX11* mShaders[GHShaderType::ST_MAX];

	D3D11_RASTERIZER_DESC mRasterizerDesc;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
	D3D11_DEPTH_STENCIL_DESC mDepthStencilDesc;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	D3D11_BLEND_DESC mBlendDesc;
	Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
};
