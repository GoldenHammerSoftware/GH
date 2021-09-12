// Copyright Golden Hammer Software
#pragma once

#include "GHDX11Include.h"
#include "GHShaderDX11.h"

class GHRenderDeviceDX11;

// A class to prevent some duplicate render state changes
class GHRenderStateMgrDX11
{
public:
	GHRenderStateMgrDX11(GHRenderDeviceDX11& device);

	void applyShader(GHShaderResource* shader, GHShaderDX11::ShaderType type);
	void applyRasterizerState(Microsoft::WRL::ComPtr<ID3D11RasterizerState> state);
	void applyDepthStencilState(Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state);
	void applyBlendState(Microsoft::WRL::ComPtr<ID3D11BlendState> state);
	// should be called during frame begin.
	void clearStates(void);

private:
	GHRenderDeviceDX11& mDevice;

	GHShaderResource* mShaders[GHShaderDX11::ST_MAX];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
};