// Copyright Golden Hammer Software
#include "GHRenderStateMgrDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHShaderType.h"

GHRenderStateMgrDX11::GHRenderStateMgrDX11(GHRenderDeviceDX11& device)
: mDevice(device)
, mRasterizerState(nullptr)
, mDepthStencilState(nullptr)
, mBlendState(nullptr)
{
	clearStates();
}

void GHRenderStateMgrDX11::applyShader(GHShaderResource* shader, GHShaderType::Enum type)
{
	if (mShaders[type] == shader) return;
	mShaders[type] = shader;
	shader->get()->bind();
}

void GHRenderStateMgrDX11::applyRasterizerState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> state)
{
	if (mRasterizerState.Get() == state.Get()) return;
	mRasterizerState = state;
	mDevice.getD3DContext()->RSSetState(mRasterizerState.Get());
}

void GHRenderStateMgrDX11::applyDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state)
{
	if (mDepthStencilState.Get() == state.Get()) return;
	mDepthStencilState = state;
	mDevice.getD3DContext()->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
}

void GHRenderStateMgrDX11::applyBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> state)
{
	if (mBlendState.Get() == state.Get()) return;
	mBlendState = state;
	mDevice.getD3DContext()->OMSetBlendState(mBlendState.Get(), 0, 0xffffffff);
}

void GHRenderStateMgrDX11::clearStates(void)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		mShaders[shaderType] = 0;
	}
	mRasterizerState = nullptr;
	mDepthStencilState = nullptr;
	mBlendState = nullptr;
}
