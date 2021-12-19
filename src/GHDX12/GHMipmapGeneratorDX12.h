#pragma once

#include "GHDX12Include.h"
#include "GHShaderDX12.h"

class GHResourceFactory;
class GHRenderDeviceDX12;

class GHMipmapGeneratorDX12
{
public:
	GHMipmapGeneratorDX12(GHResourceFactory& resourceFactory, GHRenderDeviceDX12& device);
	~GHMipmapGeneratorDX12(void);

	void generateMipmaps(Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, uint32_t width, uint32_t height);

private:
	void createGraphicsRootSignature(void);

private:
	GHRenderDeviceDX12& mDevice;
	GHShaderResource* mShader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mGraphicsRootSignature{ nullptr };
};
